#include "DirectXLib.h"


bool DirectX::initialDirectX(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	//1.�����ӿ�
	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	//2.��ȡӲ����Ϣ��ȷ�����㴦��ʽ
	D3DCAPS9 caps;
	int vertexProcessing;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);    //(��ǰ�Կ���Ӳ���豸)
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//3.��дD3D����
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow              = hWnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true;
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//4.�����豸
	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DXDevice);
	d3d9->Release();

	pD3DXDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pD3DSurface, 0);
	

	//������Ļ����
	width_ = width;
	height_ = height;

	return true;
}

void DirectX::fillSurface()
{
	pD3DXDevice->ColorFill(pD3DSurface,NULL,D3DCOLOR_XRGB(0,0,0));
}
void DirectX::lockSurface()
{
	// ��������ʼ����������
	memset(&LockRect, 0, sizeof(LockRect));
	// ����
	pD3DSurface->LockRect(&LockRect,NULL,D3DLOCK_DISCARD);

}

void DirectX::unlockSurface()
{
	// ����
	pD3DSurface->UnlockRect();
}
void DirectX::drawPixel(int x,int y, Color color)
{
	/* ������ɫ
	Pointer to the locked bits. 
	If a RECT was provided to the LockRect call, 
	pBits will be appropriately offset from the start of the surface.*/
	DWORD* pBits = (DWORD*)LockRect.pBits;
	pBits[x + y * (LockRect.Pitch >> 2)] = ARGB( color.a_ , color.r_ , color.g_ , color.b_ );      

}

/************************************************************************/
/* ����ֱ��(�е㷨)
 * ��ʽ����f(x,y)=(y0-y1)x+(x1-x0)y+x0y1-x1y0=0
 */
/************************************************************************/
void DirectX::drawLine(int x1,int y1,int x2,int y2,Color color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy)
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float A = y2-y1;
		float B = x1-x2;
		float C = x2*y1-x1*y2;
		int incrementY = (y2>y1)?1:-1;
		for (int x=x1,y=y1;x<=x2;++x)
		{
			drawPixel(x,y,color);
			float k = A*(x+1)+B*(y+incrementY)+C;
			if (k*incrementY>=0)
			{
				y += incrementY;
			}
		}
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float A = y2-y1;
		float B = x1-x2;
		float C = x2*y1-x1*y2;
		int incrementX = (x2>x1)?1:-1;
		for (int x=x1,y=y1;y<=y2;++y)
		{
			drawPixel(x,y,color);
			float k = A*(x+incrementX)+B*(y+1)+C;
			if (k*incrementX<=0)
			{
				x += incrementX;
			}
		}
	}	
}

/************************************************************************/
/* �������ζ�����y��С��������                                            */
/************************************************************************/
void DirectX::sortTriangleVector2( Vector2 &v1, Vector2 &v2, Vector2 &v3)
{
	if (v1.y>v2.y)
	{
		swap(v1,v2);
	}
	if (v3.y<v1.y)
	{
		swap(v1,v3);
		swap(v2,v3);
	}
	else if(v3.y<v2.y)
	{
		swap(v2,v3);
	}
}

/************************************************************************/
/* ���Ʋ�ֵ����ɫ�仯��ֱ��                                               */
/************************************************************************/
void DirectX::drawScanLine( Vector2 &v1, Vector2 &v2)
{
	if (v1.x>v2.x)
	{
		swap(v1,v2);
	}
	int x_start = v1.x;
	int x_end = v2.x;
	Color color = v1.color;
	Color d_color = v2.color-v1.color;
	bool isZero = x_end-x_start?false:true;
	for (int x=x_start;x<=x_end;++x)
	{
		float factor = 0;
		if (!isZero)
		{
			factor = (float(x-x_start))/(x_end-x_start);
		}
		drawPixel(x,v1.y,color);
		color = v1.color + d_color*factor; 
	}
}
/************************************************************************/
/* ���Ƶ�ƽ������	 v1Ϊ�϶���	(v2���£�v3����)		
*/
/************************************************************************/
void DirectX::drawTriangleBottomFlat( Vector2 &v1, Vector2 &v2, Vector2 &v3)
{
	if (v2.x>v3.x)
	{
		swap(v2,v3);
	}
	int y = v1.y;
	int startY = v1.y;
	int endY = v2.y;
	float LX = v1.x,RX = v1.x;
	Color d_left_color = v2.color-v1.color;
	Color d_right_color = v3.color-v1.color;
	for (;y<=endY;++y)
	{
		float factor = 0;
		if (startY-endY!=0)
		{
			factor = (float(y-startY))/(endY-startY);
		}
		LX = v1.x+(v2.x-v1.x)*factor;
		RX = v1.x+(v3.x-v1.x)*factor;
		Color  co = d_left_color*factor;
		Color left_color = v1.color + d_left_color*factor;
		Color right_color = v1.color + d_right_color*factor;
		drawScanLine(Vector2(LX,y,left_color),Vector2(RX,y,right_color));

	}
}

/************************************************************************/
/* ���ƶ�ƽ������		v3Ϊ�׶���	(v1���ϣ�v2����)		
*/
/************************************************************************/
void DirectX::drawTriangleTopFlat(Vector2 &v1, Vector2 &v2, Vector2 &v3)
{
	if (v1.x>v2.x)
	{
		swap(v1,v2);
	}
	int y = v3.y;
	int startY = v3.y;
	int endY = v2.y;
	int LX = v3.x,RX = v3.x;

	for (;y>=endY;--y)
	{	
		float factor =0;
		if (startY-endY!=0)
		{
			factor = (float(startY-y))/(startY-endY);
		}
		LX = v3.x+(v1.x-v3.x)*factor;
		RX = v3.x+(v2.x-v3.x)*factor;
		Color left_color = v3.color + (v1.color-v3.color)*factor;
		Color right_color = v3.color + (v2.color-v3.color)*factor;
		drawScanLine(Vector2(LX,y,left_color),Vector2(RX,y,right_color));

	}
}

void DirectX::drawTriangle( Vector2 &v1, Vector2 &v2, Vector2 &v3)
{
	sortTriangleVector2(v1,v2,v3);
	if (v1==v2&&v2==v3)
	{
		drawPixel(v1.x,v2.x,v1.color);
	}
	else if (v1==v2)
	{
		drawLine(v1.x,v1.y,v3.x,v3.y,v1.color);
	}
	else if(v1==v3)
	{
		 drawLine(v1.x,v1.y,v2.x,v2.y,v1.color);
	}
	else if (v2==v3)
	{
		drawLine(v1.x,v1.y,v3.x,v3.y,v1.color);
	}
	else
	{
		if (v1.y==v2.y)
		{
			drawTriangleTopFlat(v1,v2,v3);
		}
		else if (v2.y==v3.y)
		{
			drawTriangleBottomFlat(v1,v2,v3);
		}
		else
		{
			Color color = v3.color + (v1.color-v3.color)*(v2.y-v3.y)/(v1.y-v3.y);
			Vector2 v4(v1.x+(float(v2.y-v1.y)/(v3.y-v1.y))*(v3.x-v1.x),v2.y,color);
			drawTriangleBottomFlat(v1,v2,v4);
			drawTriangleTopFlat(v2,v4,v3);
		}
	}
}

bool is_out(TrangleIndex &triangle,const set<int> &remove_vertex_index)
{
	for (auto i : triangle.indices)
	{
		if (remove_vertex_index.count(i))
		{
			return true;
		}
	}
	return false;
}

void DirectX::draw_wireframe_model(Model& model,const set<int> &remove_vertex_index,const set<int> &remove_triangle_index)
{
	int index = 0;
	for (int index=0;index<model.poly_indices_.size();++index)
	{
		//�޳�������׶��
		if (remove_triangle_index.count(index)||is_out(model.poly_indices_[index],remove_vertex_index))
		{
			continue;
		}
		Vertex v1 = model.trans_vertexes_[model.poly_indices_[index].indices[0]];
		Vertex v2 = model.trans_vertexes_[model.poly_indices_[index].indices[1]];
		Vertex v3 = model.trans_vertexes_[model.poly_indices_[index].indices[2]];
		drawLine(v1.position_.x_,v1.position_.y_,v2.position_.x_,v2.position_.y_,Color(0,255,0,0));
		drawLine(v3.position_.x_,v3.position_.y_,v2.position_.x_,v2.position_.y_,Color(0,255,0,0));
		drawLine(v1.position_.x_,v1.position_.y_,v3.position_.x_,v3.position_.y_,Color(0,255,0,0));
	}
}

void DirectX::draw_mesh_model(Model& model,const set<int> &remove_vertex_index,const set<int> &remove_triangle_index)
{
	int index = 0;
	for (int index=0;index<model.poly_indices_.size();++index)
	{
		//�޳�������׶��
		if (remove_triangle_index.count(index)||is_out(model.poly_indices_[index],remove_vertex_index))
		{
			continue;
		}
		Vertex v1 = model.trans_vertexes_[model.poly_indices_[index].indices[0]];
		Vertex v2 = model.trans_vertexes_[model.poly_indices_[index].indices[1]];
		Vertex v3 = model.trans_vertexes_[model.poly_indices_[index].indices[2]];
		
		drawTriangle(v1.to_vector2(),v2.to_vector2(),v3.to_vector2());
		//drawLine(v1.position_.x_,v1.position_.y_,v2.position_.x_,v2.position_.y_,Color(0,255,0,0));
		//drawLine(v3.position_.x_,v3.position_.y_,v2.position_.x_,v2.position_.y_,Color(0,255,0,0));
		//drawLine(v1.position_.x_,v1.position_.y_,v3.position_.x_,v3.position_.y_,Color(0,255,0,0));
	}
}


void DirectX::flipSurface()
{
	// ��ȡ��̨����
	IDirect3DSurface9* backBuffer = 0;
	pD3DXDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// ʹ���Զ����������̨����
	pD3DXDevice->StretchRect(pD3DSurface, 0, backBuffer, 0, D3DTEXF_NONE);

	// GetBackBuffer���õĻ�����Ҫ���ͷţ�������ڴ�й¶
	backBuffer->Release();

	// ���������еĺ�̨������ʾ
	pD3DXDevice->Present(NULL, NULL, NULL, NULL);
}

IDirect3DDevice9* DirectX::getDevice()
{
	return pD3DXDevice;
}

IDirect3DSurface9* DirectX::getSurface()
{
	return pD3DSurface;
}

DirectX::~DirectX()
{
	if (pD3DSurface!=NULL)
	{
		pD3DSurface->Release();
	}
	if (pD3DXDevice!=NULL)
	{
		pD3DXDevice->Release();
	}
}