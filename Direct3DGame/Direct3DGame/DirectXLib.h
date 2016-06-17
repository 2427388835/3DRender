#ifndef DIRECTXLIB_H_
#define DIRECTXLIB_H_

#include<d3d9.h>
#include<d3dx9.h>
#include <windows.h>
#include <iostream>
#include <set>
#include <vector>
#include "Math.h"
#include "Camera.h"
#include "Model.h"
#include "Debug.h"
#include "Enum.h"

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

	//����ֱ��
	void drawLine(int ,int ,int ,int,AColor  );

	//����������
	bool compare(const Vertex &v1,const Vertex &v2){return v1.position_.y_<v2.position_.y_;}
	void sortTriangleVector2( Vertex &v1, Vertex &v2, Vertex &v3);
	void drawScanLine( Vertex &v1, Vertex &v2);
	void drawTriangleBottomFlat( Vertex &v1, Vertex &v2, Vertex &v3);
	void drawTriangleTopFlat( Vertex &v1, Vertex &v2, Vertex &v3);
	void drawTriangle( Vertex &v1, Vertex &v2, Vertex &v3);

	//�����߿�ģ��
	void draw_wireframe_model(Model& model,const set<int> &remove_vertex_index,const set<int> &remove_triangle_index);
	void draw_mesh_model(Model& model,const set<int> &remove_vertex_index,const set<int> &remove_triangle_index);
	//��ȡ�豸
	IDirect3DDevice9* getDevice();

	//��ȡ������
	IDirect3DSurface9* getSurface();

	//������Ⱦģʽ
	void set_render_state(RenderState render_sate){render_state_=render_sate;}

	DWORD ARGB(int a,int r,int g,int b){return DWORD((a<<24)+(r<<16)+(g<<8)+b);}
	//����
	~DirectX();
	D3DLOCKED_RECT LockRect;
private:
	DirectX():pD3DXDevice(nullptr),pD3DSurface(nullptr),camera(Camera::instance()),render_state_(TEXTURE){}
	bool is_out(TrangleIndex &triangle,const set<int> &remove_vertex_index);

	IDirect3DDevice9* pD3DXDevice;
	IDirect3DSurface9* pD3DSurface;
	Camera &camera;
	Texture *p_texture;
	float *z_buffer_;
	int width_;
	int height_;
	int buffer_size_;
	RenderState render_state_;
	
};


#endif