#ifndef DIRECTXLIB_H_
#define DIRECTXLIB_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <iostream>

#include "Enum.h"
#include "Math.h"

using namespace std;


class DirectX
{
public:
	//������ȡ
	static DirectX& instance()
	{
		static DirectX* pDirectX = new DirectX();
		return *pDirectX;
	}
	//��ʼ��DirectX
	bool initialDirectX(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void fillSurface();
	void lockSurface();
	void unlockSurface();
	//�������ص�������
	void drawPixel(int x,int y,AColor color,float depth=0.0f);

	//��������ת��ʾ
	void flipSurface();

	//������Ⱦģʽ
	void set_render_state(RenderState render_sate){render_state_=render_sate;}


	DWORD ARGB(int a,int r,int g,int b){return DWORD((a<<24)+(r<<16)+(g<<8)+b);}
	//����
	~DirectX();

private:
	DirectX():pD3DXDevice(nullptr),pD3DSurface(nullptr),render_state_(RenderState::COLOR){}

	IDirect3DDevice9* pD3DXDevice;
	IDirect3DSurface9* pD3DSurface;
	D3DLOCKED_RECT LockRect;

	float *z_buffer_;
	int width_;
	int height_;
	int buffer_size_;
	RenderState render_state_;

};


#endif