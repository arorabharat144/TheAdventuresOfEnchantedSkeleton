//====================================================================================================
// Filename:	Gui.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Gui.h"

#include "Camera.h"
#include "ConstantBuffer.h"
#include "GraphicsSystem.h"
#include "PixelShader.h"
#include "Vertex.h"
#include "VertexShader.h"
#include <Imgui/Inc/imgui.h>

using namespace X;

namespace
{
	ID3D11Buffer* sVertexBuffer = nullptr;
	ID3D11Buffer* sIndexBuffer = nullptr;
	ID3D11Buffer* sConstantBuffer = nullptr;

	ID3D10Blob* sVertexShaderBlob = nullptr;
	ID3D11VertexShader* sVertexShader = nullptr;
	ID3D11InputLayout* sInputLayout = nullptr;

	ID3D10Blob*             sPixelShaderBlob = nullptr;
	ID3D11PixelShader*       sPixelShader = nullptr;

	ID3D11SamplerState*      g_pFontSampler = nullptr;
	ID3D11ShaderResourceView*sFontTextureView = nullptr;
	ID3D11RasterizerState*   g_pRasterizerState = nullptr;
	ID3D11BlendState*        g_pBlendState = nullptr;
	ID3D11DepthStencilState* g_pDepthStencilState = nullptr;
	
	int sVertexCount = 5000;
	int sIndexCount = 10000;

	struct GuiConstantBuffer
	{
		float mvp[4][4];
	};

	void RenderDrawLists(ImDrawData* drawData)
	{
		ID3D11Device* device = GraphicsSystem::Get()->GetDevice();
		ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();

		// Create and grow vertex/index buffers if needed
		if (!sVertexBuffer || sVertexCount < drawData->TotalVtxCount)
		{
			if (sVertexBuffer) { sVertexBuffer->Release(); sVertexBuffer = nullptr; }
			sVertexCount = drawData->TotalVtxCount + 5000;
			D3D11_BUFFER_DESC desc;
			memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sVertexCount * sizeof(ImDrawVert);
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if (device->CreateBuffer(&desc, nullptr, &sVertexBuffer) < 0)
				return;
		}
		if (!sIndexBuffer || sIndexCount < drawData->TotalIdxCount)
		{
			if (sIndexBuffer) { sIndexBuffer->Release(); sIndexBuffer = nullptr; }
			sIndexCount = drawData->TotalIdxCount + 10000;
			D3D11_BUFFER_DESC desc;
			memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sIndexCount * sizeof(ImDrawIdx);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			if (device->CreateBuffer(&desc, nullptr, &sIndexBuffer) < 0)
				return;
		}

		// Copy and convert all vertices into a single contiguous buffer
		D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
		if (context->Map(sVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
			return;
		if (context->Map(sIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
			return;
		ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
		ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = drawData->CmdLists[n];
			memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtx_dst += cmd_list->VtxBuffer.Size;
			idx_dst += cmd_list->IdxBuffer.Size;
		}
		context->Unmap(sVertexBuffer, 0);
		context->Unmap(sIndexBuffer, 0);

		// Setup orthographic projection matrix into our constant buffer
		{
			D3D11_MAPPED_SUBRESOURCE mapped_resource;
			if (context->Map(sConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
				return;
			GuiConstantBuffer* constant_buffer = (GuiConstantBuffer*)mapped_resource.pData;
			float L = 0.0f;
			float R = ImGui::GetIO().DisplaySize.x;
			float B = ImGui::GetIO().DisplaySize.y;
			float T = 0.0f;
			float mvp[4][4] =
			{
				{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
				{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
				{ 0.0f,         0.0f,           0.5f,       0.0f },
				{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
			};
			memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
			context->Unmap(sConstantBuffer, 0);
		}

		// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
		struct BACKUP_DX11_STATE
		{
			UINT                        ScissorRectsCount, ViewportsCount;
			D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			ID3D11RasterizerState*      RS;
			ID3D11BlendState*           BlendState;
			FLOAT                       BlendFactor[4];
			UINT                        SampleMask;
			UINT                        StencilRef;
			ID3D11DepthStencilState*    DepthStencilState;
			ID3D11ShaderResourceView*   PSShaderResource;
			ID3D11SamplerState*         PSSampler;
			ID3D11PixelShader*          PS;
			ID3D11VertexShader*         VS;
			UINT                        PSInstancesCount, VSInstancesCount;
			ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
			D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
			ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
			UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
			DXGI_FORMAT                 IndexBufferFormat;
			ID3D11InputLayout*          InputLayout;
		};
		BACKUP_DX11_STATE old;
		old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		context->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
		context->RSGetViewports(&old.ViewportsCount, old.Viewports);
		context->RSGetState(&old.RS);
		context->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
		context->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
		context->PSGetShaderResources(0, 1, &old.PSShaderResource);
		context->PSGetSamplers(0, 1, &old.PSSampler);
		old.PSInstancesCount = old.VSInstancesCount = 256;
		context->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
		context->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
		context->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
		context->IAGetPrimitiveTopology(&old.PrimitiveTopology);
		context->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
		context->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
		context->IAGetInputLayout(&old.InputLayout);

		// Setup viewport
		D3D11_VIEWPORT vp;
		memset(&vp, 0, sizeof(D3D11_VIEWPORT));
		vp.Width = ImGui::GetIO().DisplaySize.x;
		vp.Height = ImGui::GetIO().DisplaySize.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = vp.TopLeftY = 0.0f;
		context->RSSetViewports(1, &vp);

		// Bind shader and vertex buffers
		unsigned int stride = sizeof(ImDrawVert);
		unsigned int offset = 0;
		context->IASetInputLayout(sInputLayout);
		context->IASetVertexBuffers(0, 1, &sVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(sIndexBuffer, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetShader(sVertexShader, nullptr, 0);
		context->VSSetConstantBuffers(0, 1, &sConstantBuffer);
		context->PSSetShader(sPixelShader, nullptr, 0);
		context->PSSetSamplers(0, 1, &g_pFontSampler);

		// Setup render state
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		context->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
		context->OMSetDepthStencilState(g_pDepthStencilState, 0);
		context->RSSetState(g_pRasterizerState);

		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = drawData->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
					context->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
					context->RSSetScissorRects(1, &r);
					context->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
				}
				idx_offset += pcmd->ElemCount;
			}
			vtx_offset += cmd_list->VtxBuffer.Size;
		}

		// Restore modified DX state
		context->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
		context->RSSetViewports(old.ViewportsCount, old.Viewports);
		context->RSSetState(old.RS); if (old.RS) old.RS->Release();
		context->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
		context->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
		context->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
		context->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
		context->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
		for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
		context->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
		context->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
		for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
		context->IASetPrimitiveTopology(old.PrimitiveTopology);
		context->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
		context->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
		context->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
	}
}

void Gui::Initialize(HWND window)
{
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;	// Don't use .ini file

	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = RenderDrawLists;
	io.ImeWindowHandle = window;

	// Setup display size (need to update this every frame if we want to accommodate for window resizing)
	RECT rect;
	GetClientRect(window, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	ID3D11Device* device = GraphicsSystem::Get()->GetDevice();
	ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();

	// Create the vertex shader
	{
		const char* vertexShader =
			"cbuffer vertexBuffer : register(b0) \
			{\
				float4x4 ProjectionMatrix; \
			};\
			struct VS_INPUT\
			{\
				float2 pos : POSITION;\
				float4 col : COLOR0;\
				float2 uv  : TEXCOORD0;\
			};\
			\
			struct PS_INPUT\
			{\
				float4 pos : SV_POSITION;\
				float4 col : COLOR0;\
				float2 uv  : TEXCOORD0;\
			};\
			\
			PS_INPUT main(VS_INPUT input)\
			{\
				PS_INPUT output;\
				output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
				output.col = input.col;\
				output.uv  = input.uv;\
				return output;\
			}";

		D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &sVertexShaderBlob, NULL);
		if (sVertexShaderBlob == NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			return;
		if (device->CreateVertexShader((DWORD*)sVertexShaderBlob->GetBufferPointer(), sVertexShaderBlob->GetBufferSize(), NULL, &sVertexShader) != S_OK)
			return;

		// Create the input layout
		D3D11_INPUT_ELEMENT_DESC local_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (device->CreateInputLayout(local_layout, 3, sVertexShaderBlob->GetBufferPointer(), sVertexShaderBlob->GetBufferSize(), &sInputLayout) != S_OK)
			return;

		// Create the constant buffer
		{
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = sizeof(GuiConstantBuffer);
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if (device->CreateBuffer(&desc, NULL, &sConstantBuffer) != S_OK)
				return;
		}
	}

	// Create the pixel shader
	{
		const char* pixelShader =
			"struct PS_INPUT\
			{\
				float4 pos : SV_POSITION;\
				float4 col : COLOR0;\
				float2 uv  : TEXCOORD0;\
			};\
			sampler sampler0;\
			Texture2D texture0;\
			\
			float4 main(PS_INPUT input) : SV_Target\
			{\
				float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
				return out_col; \
			}";

		D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &sPixelShaderBlob, NULL);
		if (sPixelShaderBlob == NULL)  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			return;
		if (device->CreatePixelShader((DWORD*)sPixelShaderBlob->GetBufferPointer(), sPixelShaderBlob->GetBufferSize(), NULL, &sPixelShader) != S_OK)
			return;
	}

	// Create the blending setup
	{
		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		device->CreateBlendState(&desc, &g_pBlendState);
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		device->CreateRasterizerState(&desc, &g_pRasterizerState);
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		device->CreateDepthStencilState(&desc, &g_pDepthStencilState);
	}

	// Build texture atlas
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* texture = nullptr;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = pixels;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		device->CreateTexture2D(&desc, &subResource, &texture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		device->CreateShaderResourceView(texture, &srvDesc, &sFontTextureView);
		texture->Release();
	}

	// Store our identifier
	io.Fonts->TexID = (void*)sFontTextureView;

	// Create texture sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0.f;
		desc.MaxLOD = 0.f;
		device->CreateSamplerState(&desc, &g_pFontSampler);
	}
}

//----------------------------------------------------------------------------------------------------

void Gui::Terminate()
{
	SafeRelease(sVertexBuffer);
	SafeRelease(sIndexBuffer);

	SafeRelease(sVertexShaderBlob);
	SafeRelease(sVertexShader);
	SafeRelease(sInputLayout);

	SafeRelease(sPixelShaderBlob);
	SafeRelease(sPixelShader);

	SafeRelease(g_pFontSampler);
	SafeRelease(sFontTextureView);
	SafeRelease(g_pRasterizerState);
	SafeRelease(g_pBlendState);
	SafeRelease(g_pDepthStencilState);
	SafeRelease(g_pDepthStencilState);

	ImGui::Shutdown();
}

//----------------------------------------------------------------------------------------------------

void Gui::BeginRender(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup time step
	io.DeltaTime = deltaTime;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		SetCursor(nullptr);

	// Start the frame
	ImGui::NewFrame();
}

//----------------------------------------------------------------------------------------------------

void Gui::EndRender()
{
	ImGui::Render();
}