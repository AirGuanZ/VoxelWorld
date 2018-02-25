/*
 * Nuklear - 1.32.0 - public domain
 * no warrenty implied; use at your own risk.
 * authored from 2015-2016 by Micha Mettke
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_D3D11_H_
#define NK_D3D11_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct ID3D11Device ID3D11Device;
typedef struct ID3D11DeviceContext ID3D11DeviceContext;

NK_API struct nk_context *nk_d3d11_init(ID3D11Device *device, int width, int height, unsigned int max_vertex_buffer, unsigned int max_index_buffer);
NK_API void nk_d3d11_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void nk_d3d11_font_stash_end(void);
NK_API void nk_d3d11_render(ID3D11DeviceContext *context, enum nk_anti_aliasing);
NK_API void nk_d3d11_resize(ID3D11DeviceContext *context, int width, int height);
NK_API void nk_d3d11_shutdown(void);

#endif
/*
 * ==============================================================
 *
 *                          IMPLEMENTATION
 *
 * ===============================================================
 */
#ifdef NK_D3D11_IMPLEMENTATION

#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <d3d11.h>

#include <stddef.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#include "nuklear_d3d11_vertex_shader.h"
#include "nuklear_d3d11_pixel_shader.h"

struct nk_d3d11_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

static struct
{
    struct nk_context ctx;
    struct nk_font_atlas atlas;
    struct nk_buffer cmds;

    struct nk_draw_null_texture null;
    unsigned int max_vertex_buffer;
    unsigned int max_index_buffer;

    D3D11_VIEWPORT viewport;
    ID3D11Device *device;
    ID3D11RasterizerState *rasterizer_state;
    ID3D11VertexShader *vertex_shader;
    ID3D11InputLayout *input_layout;
    ID3D11Buffer *const_buffer;
    ID3D11PixelShader *pixel_shader;
    ID3D11BlendState *blend_state;
    ID3D11Buffer *index_buffer;
    ID3D11Buffer *vertex_buffer;
    ID3D11ShaderResourceView *font_texture_view;
    ID3D11SamplerState *sampler_state;
} d3d11;

NK_API void
nk_d3d11_render(ID3D11DeviceContext *context, enum nk_anti_aliasing AA)
{
    const float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const UINT stride = sizeof(struct nk_d3d11_vertex);
    const UINT offset = 0;

    context->IASetInputLayout(d3d11.input_layout);
    context->IASetVertexBuffers(0, 1, &d3d11.vertex_buffer, &stride, &offset);
    context->IASetIndexBuffer(d3d11.index_buffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(d3d11.vertex_shader, NULL, 0);
    context->VSSetConstantBuffers(0, 1, &d3d11.const_buffer);

    context->PSSetShader(d3d11.pixel_shader, NULL, 0);
    context->PSSetSamplers(0, 1, &d3d11.sampler_state);

    context->OMSetBlendState(d3d11.blend_state, blend_factor, 0xffffffff);
    context->RSSetState(d3d11.rasterizer_state);
    context->RSSetViewports(1, &d3d11.viewport);

    /* Convert from command queue into draw list and draw to screen */
    {/* load draw vertices & elements directly into vertex + element buffer */
    D3D11_MAPPED_SUBRESOURCE vertices;
    D3D11_MAPPED_SUBRESOURCE indices;
    const struct nk_draw_command *cmd;
    UINT offset = 0;
    HRESULT hr;

    hr = context->Map((ID3D11Resource *)d3d11.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertices);
    NK_ASSERT(SUCCEEDED(hr));
    hr = context->Map((ID3D11Resource *)d3d11.index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &indices);
    NK_ASSERT(SUCCEEDED(hr));

    {/* fill converting configuration */
        struct nk_convert_config config;
        NK_STORAGE const struct nk_draw_vertex_layout_element vertex_layout[] = {
            {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_d3d11_vertex, position)},
            {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_d3d11_vertex, uv)},
            {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_d3d11_vertex, col)},
            {NK_VERTEX_LAYOUT_END}
        };
        memset(&config, 0, sizeof(config));
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(struct nk_d3d11_vertex);
        config.vertex_alignment = NK_ALIGNOF(struct nk_d3d11_vertex);
        config.global_alpha = 1.0f;
        config.shape_AA = AA;
        config.line_AA = AA;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.null = d3d11.null;

        {/* setup buffers to load vertices and elements */
        struct nk_buffer vbuf, ibuf;
        nk_buffer_init_fixed(&vbuf, vertices.pData, (size_t)d3d11.max_vertex_buffer);
        nk_buffer_init_fixed(&ibuf, indices.pData, (size_t)d3d11.max_index_buffer);
        nk_convert(&d3d11.ctx, &d3d11.cmds, &vbuf, &ibuf, &config);}
    }

    context->Unmap((ID3D11Resource *)d3d11.vertex_buffer, 0);
    context->Unmap((ID3D11Resource *)d3d11.index_buffer, 0);

    /* iterate over and execute each draw command */
    nk_draw_foreach(cmd, &d3d11.ctx, &d3d11.cmds)
    {
        D3D11_RECT scissor;
        ID3D11ShaderResourceView *texture_view = (ID3D11ShaderResourceView *)cmd->texture.ptr;
        if (!cmd->elem_count) continue;

        scissor.left = (LONG)cmd->clip_rect.x;
        scissor.right = (LONG)(cmd->clip_rect.x + cmd->clip_rect.w);
        scissor.top = (LONG)cmd->clip_rect.y;
        scissor.bottom = (LONG)(cmd->clip_rect.y + cmd->clip_rect.h);

        context->PSSetShaderResources(0, 1, &texture_view);
        context->RSSetScissorRects(1, &scissor);
        context->DrawIndexed((UINT)cmd->elem_count, offset, 0);
        offset += cmd->elem_count;
    }
    nk_clear(&d3d11.ctx);}

    context->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
    context->RSSetState(nullptr);
}

static void
nk_d3d11_get_projection_matrix(int width, int height, float *result)
{
    const float L = 0.0f;
    const float R = (float)width;
    const float T = 0.0f;
    const float B = (float)height;
    float matrix[4][4] =
    {
        {    2.0f / (R - L),              0.0f, 0.0f, 0.0f },
        {              0.0f,    2.0f / (T - B), 0.0f, 0.0f },
        {              0.0f,              0.0f, 0.5f, 0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
    };
    memcpy(result, matrix, sizeof(matrix));
}

NK_API void
nk_d3d11_resize(ID3D11DeviceContext *context, int width, int height)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map((ID3D11Resource *)d3d11.const_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        nk_d3d11_get_projection_matrix(width, height, (float *)mapped.pData);
        context->Unmap((ID3D11Resource *)d3d11.const_buffer, 0);

        d3d11.viewport.Width = (float)width;
        d3d11.viewport.Height = (float)height;
    }
}

static void
nk_d3d11_clipbard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    (void)usr;
    if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(NULL))
    {
        HGLOBAL mem = GetClipboardData(CF_UNICODETEXT); 
        if (mem)
        {
            SIZE_T size = GlobalSize(mem) - 1;
            if (size)
            {
                LPCWSTR wstr = (LPCWSTR)GlobalLock(mem);
                if (wstr)
                {
                    int utf8size = WideCharToMultiByte(CP_UTF8, 0, wstr, size / sizeof(wchar_t), NULL, 0, NULL, NULL);
                    if (utf8size)
                    {
                        char* utf8 = (char*)malloc(utf8size);
                        if (utf8)
                        {
                            WideCharToMultiByte(CP_UTF8, 0, wstr, size / sizeof(wchar_t), utf8, utf8size, NULL, NULL);
                            nk_textedit_paste(edit, utf8, utf8size);
                            free(utf8);
                        }
                    }
                    GlobalUnlock(mem); 
                }
            }
        }
        CloseClipboard();
    }
}

static void
nk_d3d11_clipbard_copy(nk_handle usr, const char *text, int len)
{
    (void)usr;
    if (OpenClipboard(NULL))
    {
        int wsize = MultiByteToWideChar(CP_UTF8, 0, text, len, NULL, 0);
        if (wsize)
        {
            HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (wsize + 1) * sizeof(wchar_t));
            if (mem)
            {
                wchar_t* wstr = (wchar_t*)GlobalLock(mem);
                if (wstr)
                {
                    MultiByteToWideChar(CP_UTF8, 0, text, len, wstr, wsize);
                    wstr[wsize] = 0;
                    GlobalUnlock(mem);
                    SetClipboardData(CF_UNICODETEXT, mem); 
                }
            }
        }
        CloseClipboard();
    }
}

NK_API struct nk_context*
nk_d3d11_init(ID3D11Device *device, int width, int height, unsigned int max_vertex_buffer, unsigned int max_index_buffer)
{
    HRESULT hr;
    d3d11.max_vertex_buffer = max_vertex_buffer;
    d3d11.max_index_buffer = max_index_buffer;
    d3d11.device = device;
    device->AddRef();

    nk_init_default(&d3d11.ctx, 0);
    d3d11.ctx.clip.copy = nk_d3d11_clipbard_copy;
    d3d11.ctx.clip.paste = nk_d3d11_clipbard_paste;
    d3d11.ctx.clip.userdata = nk_handle_ptr(0);

    nk_buffer_init_default(&d3d11.cmds);

    {/* rasterizer state */
    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_NONE;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = TRUE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;
    hr = device->CreateRasterizerState(&desc, &d3d11.rasterizer_state);
    NK_ASSERT(SUCCEEDED(hr));}

    /* vertex shader */
    {hr = device->CreateVertexShader(nk_d3d11_vertex_shader, sizeof(nk_d3d11_vertex_shader), NULL, &d3d11.vertex_shader);
    NK_ASSERT(SUCCEEDED(hr));}

    /* input layout */
    {const D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(struct nk_d3d11_vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(struct nk_d3d11_vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, offsetof(struct nk_d3d11_vertex, col),      D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), nk_d3d11_vertex_shader, sizeof(nk_d3d11_vertex_shader), &d3d11.input_layout);
    NK_ASSERT(SUCCEEDED(hr));}

    /* constant buffer */
    {float matrix[4*4];
    D3D11_BUFFER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.ByteWidth = sizeof(matrix);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    {D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = matrix;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    nk_d3d11_get_projection_matrix(width, height, matrix);
    hr = device->CreateBuffer(&desc, &data, &d3d11.const_buffer);
    NK_ASSERT(SUCCEEDED(hr));}}

    /* pixel shader */
    {hr = device->CreatePixelShader(nk_d3d11_pixel_shader, sizeof(nk_d3d11_pixel_shader), NULL, &d3d11.pixel_shader);
    NK_ASSERT(SUCCEEDED(hr));}

    {/* blend state */
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = FALSE;
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&desc, &d3d11.blend_state);
    NK_ASSERT(SUCCEEDED(hr));}

    /* vertex buffer */
    {D3D11_BUFFER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = max_vertex_buffer;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    hr = device->CreateBuffer(&desc, NULL, &d3d11.vertex_buffer);
    NK_ASSERT(SUCCEEDED(hr));}

    /* index buffer */
    {D3D11_BUFFER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = max_index_buffer;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&desc, NULL, &d3d11.index_buffer);
    NK_ASSERT(SUCCEEDED(hr));}

    /* sampler state */
    {D3D11_SAMPLER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.MipLODBias = 0.0f;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.MinLOD = 0.0f;
    desc.MaxLOD = FLT_MAX;
    hr = device->CreateSamplerState(&desc, &d3d11.sampler_state);
    NK_ASSERT(SUCCEEDED(hr));}

    /* viewport */
    {d3d11.viewport.TopLeftX = 0.0f;
    d3d11.viewport.TopLeftY = 0.0f;
    d3d11.viewport.Width = (float)width;
    d3d11.viewport.Height = (float)height;
    d3d11.viewport.MinDepth = 0.0f;
    d3d11.viewport.MaxDepth = 1.0f;}
    return &d3d11.ctx;
}

NK_API void
nk_d3d11_font_stash_begin(struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&d3d11.atlas);
    nk_font_atlas_begin(&d3d11.atlas);
    *atlas = &d3d11.atlas;
}

NK_API void
nk_d3d11_font_stash_end(void)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&d3d11.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    /* upload font to texture and create texture view */
    {ID3D11Texture2D *font_texture;
    HRESULT hr;

    D3D11_TEXTURE2D_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = (UINT)w;
    desc.Height = (UINT)h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    {D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = image;
    data.SysMemPitch = (UINT)(w * 4);
    data.SysMemSlicePitch = 0;
    hr = d3d11.device->CreateTexture2D(&desc, &data, &font_texture);
    assert(SUCCEEDED(hr));}

    {D3D11_SHADER_RESOURCE_VIEW_DESC srv;
    memset(&srv, 0, sizeof(srv));
    srv.Format = desc.Format;
    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = 1;
    srv.Texture2D.MostDetailedMip = 0;
    hr = d3d11.device->CreateShaderResourceView((ID3D11Resource *)font_texture, &srv, &d3d11.font_texture_view);
    assert(SUCCEEDED(hr));}
    font_texture->Release();}

    nk_font_atlas_end(&d3d11.atlas, nk_handle_ptr(d3d11.font_texture_view), &d3d11.null);
    if (d3d11.atlas.default_font)
        nk_style_set_font(&d3d11.ctx, &d3d11.atlas.default_font->handle);
}

NK_API
void nk_d3d11_shutdown(void)
{
    nk_font_atlas_clear(&d3d11.atlas);
    nk_buffer_free(&d3d11.cmds);
    nk_free(&d3d11.ctx);

    d3d11.sampler_state->Release();
    d3d11.font_texture_view->Release();
    d3d11.vertex_buffer->Release();
    d3d11.index_buffer->Release();
    d3d11.blend_state->Release();
    d3d11.pixel_shader->Release();
    d3d11.const_buffer->Release();
    d3d11.vertex_shader->Release();
    d3d11.input_layout->Release();
    d3d11.rasterizer_state->Release();
    d3d11.device->Release();
}

#endif

