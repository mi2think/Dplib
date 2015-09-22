/********************************************************************
	created:	2015/09/04
	created:	4:9:2015   23:31
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTriangle.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTriangle
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Triangle
*********************************************************************/


/*
 * INTERP_Z			- if defined, interpolate Z values
 * INTERP_INVZ		- if defined, interpolate 1/Z values
 * INTERP_RGB		- if defined, interpolate RGB values
 * INTERP_UV		- if defined, interpolate UV values
 * INTERP_UV_DIVZ	- if defined, interpolate UV values
 *
 */

//void Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	// helper class
	struct Gradients
	{
#if INTERP_RGB
		float drdx; float drdy;
		float dgdx; float dgdy;
		float dbdx; float dbdy;
#endif
#if INTERP_Z || INTERP_INVZ
		float z[3];					// z for each vertex ; or 1/z for each vertex
		float dzdx; float dzdy;		// dz/dx, dz/dy ; or d(1/z)/dx, d(1/z)/dy
#endif
#if INTERP_UV || INTERP_UV_DIVZ
		float u[3];					// u for each vertex ; or u/z for each vertex
		float v[3];					// v for each vertex ; or v/z for each vertex
		float dudx; float dudy;		// du/dx, du/dy ; or d(u/z)/dx, d(u/z)/dy
		float dvdx; float dvdy;		// dv/dx, dv/dy ; or d(v/z)/dx, d(v/z)/dy
#endif

		Gradients(const Vertex** pVertices)
		{
			const int counter = 3;

			// common sub compute
			float y0_y2 = pVertices[0]->y - pVertices[2]->y;	// y0 - y2
			float y1_y2 = pVertices[1]->y - pVertices[2]->y;	// y1 - y2
			float x0_x2 = pVertices[0]->x - pVertices[2]->x;	// x0 - x2
			float x1_x2 = pVertices[1]->x - pVertices[2]->x;	// x1 - x2

			// equation:
			// dx = (x1-x2)(y0-y2) + (x2-x0)(y1-y2)
			// dy = -dx
			float dx = x1_x2 * y0_y2 - x0_x2 * y1_y2;
			float oneOverdx = 1.0f / dx;
			float oneOverdy = -oneOverdx;

			// equation:
			// dc / dx = ((c1-c2)(y0-y2) - (c0-c2)(y1-y2)) / dx
			// dc / dy = ((c1-c2)(x0-x2) - (c0-c2)(x1-x2)) / dy
#if INTERP_RGB
			float r[3]; float g[3]; float b[3];
			for (int i = 0; i < counter; ++i)
			{
				const auto& color = pVertices[i]->color;
				r[i] = color.r;
				g[i] = color.g;
				b[i] = color.b;
			}
			drdx = oneOverdx * ((r[1] - r[2]) * y0_y2 - (r[0] - r[2]) * y1_y2);
			dgdx = oneOverdx * ((g[1] - g[2]) * y0_y2 - (g[0] - g[2]) * y1_y2);
			dbdx = oneOverdx * ((b[1] - b[2]) * y0_y2 - (b[0] - b[2]) * y1_y2);

			drdy = oneOverdy * ((r[1] - r[2]) * x0_x2 - (r[0] - r[2]) * x1_x2);
			dgdy = oneOverdy * ((g[1] - g[2]) * x0_x2 - (g[0] - g[2]) * x1_x2);
			dbdy = oneOverdy * ((b[1] - b[2]) * x0_x2 - (b[0] - b[2]) * x1_x2);
#endif
#if INTERP_Z || INTERP_INVZ
#if INTERP_Z
			z[0] = pVertices[0]->z;
			z[1] = pVertices[1]->z;
			z[2] = pVertices[2]->z;
#endif
#if INTERP_INVZ
			z[0] = 1.0f / pVertices[0]->z;
			z[1] = 1.0f / pVertices[1]->z;
			z[2] = 1.0f / pVertices[2]->z;
#endif
			dzdx = oneOverdx * ((z[1] - z[2]) * y0_y2 - (z[0] - z[2]) * y1_y2);
			dzdy = oneOverdy * ((z[1] - z[2]) * x0_x2 - (z[0] - z[2]) * x1_x2);
#endif
#if INTERP_UV || INTERP_UV_DIVZ
			for (int i = 0; i < counter; ++i)
			{
				u[i] = pVertices[i]->u0;
				v[i] = pVertices[i]->v0;
#if INTERP_UV_DIVZ
				// INTERP_INVZ must defined before
				u[i] *= z[i];
				v[i] *= z[i];
#endif
			}
			dudx = oneOverdx * ((u[1] - u[2]) * y0_y2 - (u[0] - u[2]) * y1_y2);
			dudy = oneOverdy * ((u[1] - u[2]) * x0_x2 - (u[0] - u[2]) * x1_x2);

			dvdx = oneOverdx * ((v[1] - v[2]) * y0_y2 - (v[0] - v[2]) * y1_y2);
			dvdy = oneOverdy * ((v[1] - v[2]) * x0_x2 - (v[0] - v[2]) * x1_x2);
#endif
		}
	};

	struct Edge
	{
		float x; float dxdy;		// current x and dx/dy
		int y; int height;			// current y and vertical count
#if INTERP_RGB
		float r; float drdy;		// current r, g, b
		float g; float dgdy;		// and dr/dy, dg/dy, db/dy
		float b; float dbdy;
#endif
#if INTERP_Z || INTERP_INVZ
		float z; float dzdy;		// current z, dz/dy ; or current 1/z, d(1/z)/dy
#endif
#if INTERP_UV || INTERP_UV_DIVZ
		float u; float dudy;		// u, du/dy ; or u/z, d(u/z)/dy
		float v; float dvdy;		// v, dv/dy ; or v/z, d(v/z)/dy
#endif

		Edge(const Gradients& gradients, const Vertex** pVertices, int top, int bottom)
		{
			const Vertex* vtop = *(pVertices + top);
			const Vertex* vbottom = *(pVertices + bottom);

			y = (int)ceil(vtop->y);
			int yend = (int)ceil(vbottom->y);
			height = yend - y;

			float y_pre_step = y - vtop->y;

			float real_width = vbottom->x - vtop->x;
			float real_height = vbottom->y - vtop->y;

			dxdy = real_width / real_height;
			x = vtop->x + y_pre_step * dxdy;
			float x_pre_step = x - vtop->x;
#if INTERP_RGB
			drdy = gradients.drdy + dxdy * gradients.drdx;
			dgdy = gradients.dgdy + dxdy * gradients.dgdx;
			dbdy = gradients.dbdy + dxdy * gradients.dbdx;

			r = vtop->color.r + y_pre_step * drdy + x_pre_step * gradients.drdx;
			g = vtop->color.g + y_pre_step * dgdy + x_pre_step * gradients.drdx;
			b = vtop->color.b + y_pre_step * dbdy + x_pre_step * gradients.drdx;
#endif
#if INTERP_Z || INTERP_INVZ
			// z or 1/z step depend on both y step and x step
			dzdy = gradients.dzdy + dxdy * gradients.dzdx;
			z = gradients.z[top] + y_pre_step * gradients.dzdy + x_pre_step * gradients.dzdx;
#endif
#if INTERP_UV || INTERP_UV_DIVZ
			dudy = gradients.dudy + dxdy * gradients.dudx;
			dvdy = gradients.dvdy + dxdy * gradients.dvdx;
			u = gradients.u[top] + y_pre_step * gradients.dudy + x_pre_step * gradients.dudx;
			v = gradients.v[top] + y_pre_step * gradients.dvdy + x_pre_step * gradients.dvdx;
#endif
		}

		int Step()
		{
			x += dxdy;
			++y;
			--height;
#if INTERP_RGB
			r += drdy;
			g += dgdy;
			b += dbdy;
#endif
#if INTERP_Z || INTERP_INVZ
			z += dzdy;
#endif
#if INTERP_UV || INTERP_UV_DIVZ
			u += dudy;
			v += dvdy;
#endif
			return height;
		}
	};

	// begin deal
	{
		const Vertex* pVertices[3] = { &v0, &v1, &v2 };

		int top, middle, bottom;
		float y0 = pVertices[0]->y;
		float y1 = pVertices[1]->y;
		float y2 = pVertices[2]->y;

		// sort vertices in y
		if (y0 < y1) {
			if (y2 < y0) {
				top = 2; middle = 0; bottom = 1;
			}
			else {
				top = 0;
				if (y1 < y2) {
					middle = 1; bottom = 2;
				}
				else {
					middle = 2; bottom = 1;
				}
			}
		}
		else {
			if (y2 < y1) {
				top = 2; middle = 1; bottom = 0;
			}
			else {
				top = 1;
				if (y0 < y2) {
					middle = 0; bottom = 2;
				}
				else {
					middle = 2; bottom = 0;
				}
			}
		}
		// middle is left or right?
		float xnew = pVertices[top]->x + (pVertices[bottom]->x - pVertices[top]->x) * (pVertices[middle]->y - pVertices[top]->y) / (pVertices[bottom]->y - pVertices[top]->y);
		bool middleIsLeft = (xnew > pVertices[middle]->x);

		Gradients gradients(pVertices);
		Edge edgeTB(gradients, pVertices, top, bottom);		// edge - top to bottom
		Edge edgeTM(gradients, pVertices, top, middle);		// edge - top to middle
		Edge edgeMB(gradients, pVertices, middle, bottom);	// edge - middle to bottom
		Color color = v0.color;

		// sub triangles
		unsigned int pitchBits = pitch * bitsPerPixel;
		for (int subTriangle = 0; subTriangle <= 1; ++subTriangle)
		{
			Edge* edgeL; // edge - left
			Edge* edgeR; // edge - right
			int height;

			if (subTriangle == 0) {
				// top half
				if (middleIsLeft) {
					edgeL = &edgeTM; edgeR = &edgeTB;
				} else {
					edgeL = &edgeTB; edgeR = &edgeTM;
				}
				height = edgeTM.height;
			} else {
				// bottom half
				if (middleIsLeft) {
					edgeL = &edgeMB; edgeR = &edgeTB;
				} else {
					edgeL = &edgeTB; edgeR = &edgeMB;
				}
				height = edgeMB.height;
			}

			// scan line
			unsigned char* buffer = frameBuffer + edgeL->y * pitchBits;
			float* bufferz = zbuffer.buffer + edgeL->y * zbuffer.width;
			while (height--)
			{
				// test y clip
				int y = edgeL->y;
				if (y >= clipRect.top && y < clipRect.bottom)
				{
					int x_start = (int)ceil(edgeL->x);
					int width = (int)ceil(edgeR->x) - x_start;
					float x_pre_step = x_start - edgeL->x;
#if INTERP_RGB
					float r = edgeL->r + x_pre_step * gradients.drdx;
					float g = edgeL->g + x_pre_step * gradients.dgdx;
					float b = edgeL->b + x_pre_step * gradients.dbdx;
#endif
#if INTERP_Z || INTERP_INVZ
					float z = edgeL->z + x_pre_step * gradients.dzdx;
#endif
#if INTERP_UV || INTERP_UV_DIVZ
					float u = edgeL->u + x_pre_step * gradients.dudx;
					float v = edgeL->v + x_pre_step * gradients.dvdx;
#endif
					if (width > 0)
					{
						int x = x_start;
						unsigned int* p = (unsigned int*)(buffer + x * bitsPerPixel);
						while (width--)
						{
							bool zpass = true;
							// test x clip
							if (x >= clipRect.left && x < clipRect.right)
							{
								// test z pass
#if INTERP_Z || INTERP_INVZ
								if (zEnable != ZEnable_False)
								{
									float& zf = *(bufferz + x);
									switch (zFunc)
									{
									case CMP_Never:
									default:
										zpass = false;
										break;
									case CMP_Less:
										zpass = (z < zf);
										break;
									case CMP_Equal:
										zpass = FCMP(z, zf);
										break;
									case CMP_LessEqual:
										zpass = ((z < zf) || FCMP(z, zf));
										break;
									case CMP_Greater:
										zpass = (z > zf);
										break;
									case CMP_NotEqual:
										zpass = (!FCMP(z, zf));
										break;
									case CMP_GreaterEqual:
										zpass = ((z > zf) || FCMP(z, zf));
										break;
									case CMP_Always:
										zpass = true;
										break;
									}
#if INTERP_INVZ
									// inv z
									if (zFunc != CMP_Never && zFunc != CMP_Always)
										zpass = !zpass;
#endif
									if (zpass && zWriteEnable == True)
										zf = z;
								}
#endif
#if INTERP_RGB
								unsigned char rc = (unsigned char)math::Clamp<float>(r + 0.5f, 0.0f, 255.0f);
								unsigned char gc = (unsigned char)math::Clamp<float>(g + 0.5f, 0.0f, 255.0f);
								unsigned char bc = (unsigned char)math::Clamp<float>(b + 0.5f, 0.0f, 255.0f);
								color.Set(rc, gc, bc);
#endif
#if INTERP_UV || INTERP_UV_DIVZ
								// r : mean reality
								float ur = u;
								float vr = v;
#if INTERP_UV_DIVZ
								// INTERP_INVZ must defined before
								// here: u : u/z, v : v/z, z : 1.0f/z
								float zr = 1.0f / z;
								ur *= zr;
								vr *= zr;
#endif
								ur = math::Clamp<float>(ur, 0.0f, 1.0f);
								vr = math::Clamp<float>(vr, 0.0f, 1.0f);
								texture->Sample(color, ur, vr);
#endif
								if (zpass)
								{
									*p = color.value;
								}
							}
							++x;
							++p;
#if INTERP_RGB
							r += gradients.drdx;
							g += gradients.dgdx;
							b += gradients.dbdx;
#endif
#if INTERP_Z  || INTERP_INVZ
							z += gradients.dzdx;
#endif
#if INTERP_UV || INTERP_UV_DIVZ
							u += gradients.dudx;
							v += gradients.dvdx;
#endif
						}
					}
				}
				edgeL->Step();
				edgeR->Step();
				buffer += pitchBits;
				bufferz += zbuffer.width;
			}
		}
	}
}

#undef INTERP_Z
#undef INTERP_INVZ
#undef INTERP_RGB
#undef INTERP_UV
#undef INTERP_UV_DIVZ