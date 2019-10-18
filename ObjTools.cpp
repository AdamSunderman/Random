#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <atlimage.h>
using namespace std;

struct Vertex 
{
	float x, y, z;
};

struct Color
{
	float r, g, b, a;
};

struct Tri
{
	Vertex* vertices[3];
	Vertex* normals[3];
	Vertex* texcoords[3];
	Color color;
};

class ObjTools
{
private:
	const static int MAX_BUFF_SZ = 5000;
	string object_filename, material_filename, object_name;
	Vertex* vertices[MAX_BUFF_SZ];
	Vertex* tex_coords[MAX_BUFF_SZ];
	Vertex* normals[MAX_BUFF_SZ];
	Tri* triangles[MAX_BUFF_SZ * 3];
	int vert_count, tex_count, norm_count, tri_count, tris_passed;
	CImage TextureLoader;

public:
	ObjTools::ObjTools() 
	{
		vert_count = tex_count = norm_count = tri_count = tris_passed = 0;
		TextureLoader = CImage();
	}

	ObjTools::~ObjTools() 
	{
		for (int i = 0; i < vert_count; i++) {
			delete vertices[i];
		}
		for (int j = 0; j < tex_count; j++) {
			delete tex_coords[j];
		}
		for (int k = 0; k < norm_count; k++) {
			delete normals[k];
		}
		for (int l = 0; l < tri_count; l++) {
			delete triangles[l];
		}
	}

	int ObjTools::GetTriCount() 
	{
		return tri_count;
	}

	int ObjTools::GetVertCount()
	{
		return vert_count;
	}

	int ObjTools::GetTexCoordCount()
	{
		return tex_count;
	}

	int ObjTools::GetNormCount()
	{
		return norm_count;
	}

	Tri* ObjTools::PassNextTri() 
	{
		if (tris_passed < tri_count) 
		{
			return triangles[tris_passed++];
		}
		else 
		{
			return nullptr;
		}
	}

	int ObjTools::ReadObjFile(string f) 
	{
		object_filename = f;
		string line;
		ifstream infile(f);

		if ( infile.is_open() ) 
		{
			while (getline(infile, line)) 
			{
				// Break for the line symbol
				string::size_type symbol_break = line.find_first_of(' ');
				string symbol = line.substr(0, symbol_break);
				if (symbol.compare("#") == 0)
				{
					continue;
				}
				else if (symbol.compare("mtllib") == 0)
				{
					material_filename = line.substr(symbol_break + 1, string::npos);
					//printf("%s\n", material_filename.c_str());
				}
				else if (symbol.compare("o") == 0)
				{
					object_name = line.substr(symbol_break + 1, string::npos);
					//printf("%s\n", object_name.c_str());
				}
				else if (symbol.compare("v") == 0)
				{
					string::size_type x_pos = line.find_first_of(' ', symbol_break + 1);
					string::size_type y_pos = line.find_first_of(' ', x_pos + 1);
					string::size_type z_pos = line.find_first_of(' ', y_pos + 1);
					string x_str = line.substr(symbol_break + 1, x_pos - (symbol_break + 1));
					string y_str = line.substr(x_pos + 1, y_pos - (x_pos + 1));
					string z_str = line.substr(y_pos + 1, string::npos);
					//printf("%s, %s, %s\n", x_str.c_str(), y_str.c_str(), z_str.c_str());
					float x = stof(x_str);
					float y = stof(y_str);
					float z = stof(z_str);
					//printf("%2.6f, %2.6f, %2.6f\n", x, y, z);
					Vertex* v = new Vertex();
					v->x = x;
					v->y = y;
					v->z = z;
					vertices[vert_count] = v;
					vert_count += 1;
				}
				else if (symbol.compare("vt") == 0)
				{
					string::size_type s_pos = line.find_first_of(' ', symbol_break + 1);
					string s_str = line.substr(symbol_break + 1, s_pos - (symbol_break + 1));
					string t_str = line.substr(s_pos + 1, string::npos);
					//printf("%s, %s\n", s_str.c_str(), t_str.c_str());
					float s = stof(s_str);
					float t = stof(t_str);
					//printf("%2.6f, %2.6f\n", s, t);
					Vertex* vt = new Vertex();
					vt->x = s;
					vt->y = t;
					vt->z = NULL;
					tex_coords[tex_count] = vt;
					tex_count += 1;
				}
				else if (symbol.compare("vn") == 0)
				{
					string::size_type nx_pos = line.find_first_of(' ', symbol_break + 1);
					string::size_type ny_pos = line.find_first_of(' ', nx_pos + 1);
					string::size_type nz_pos = line.find_first_of(' ', ny_pos + 1);
					string nx_str = line.substr(symbol_break + 1, nx_pos - (symbol_break + 1));
					string ny_str = line.substr(nx_pos + 1, ny_pos - (nx_pos + 1));
					string nz_str = line.substr(ny_pos + 1, string::npos);
					//printf("%s, %s, %s\n", nx_str.c_str(), ny_str.c_str(), nz_str.c_str());
					float nx = stof(nx_str);
					float ny = stof(ny_str);
					float nz = stof(nz_str);
					//printf("%2.6f, %2.6f, %2.6f\n", nx, ny, nz);
					Vertex* vn = new Vertex();
					vn->x = nx;
					vn->y = ny;
					vn->z = nz;
					normals[norm_count] = vn;
					norm_count += 1;
				}
				else if (symbol.compare("f") == 0)
				{
					string::size_type v0 = line.find_first_of(' ', symbol_break + 1);
					string::size_type v1 = line.find_first_of(' ', v0 + 1);
					string::size_type v2 = line.find_first_of(' ', v1 + 1);
					string v0_str = line.substr(symbol_break + 1, v0 - (symbol_break + 1));
					string v1_str = line.substr(v0 + 1, v1 - (v0 + 1));
					string v2_str = line.substr(v1 + 1, string::npos);
					//printf("%s, %s, %s\n", v0_str.c_str(), v1_str.c_str(), v2_str.c_str());
					string::size_type v0_split1 = v0_str.find_first_of('/');
					string::size_type v0_split2 = v0_str.find_first_of('/', v0_split1 + 1);
					string::size_type v1_split1 = v1_str.find_first_of('/');
					string::size_type v1_split2 = v1_str.find_first_of('/', v1_split1 + 1);
					string::size_type v2_split1 = v2_str.find_first_of('/');
					string::size_type v2_split2 = v2_str.find_first_of('/', v2_split1 + 1);
					//printf("%d, %d, %d, %d, %d, %d\n", v0_split1, v0_split2, v1_split1, v1_split2, v2_split1, v2_split2);
					string v0_v = v0_str.substr(0, v0_split1);
					string v0_t = v0_str.substr(v0_split1 + 1, v0_split2 - (v0_split1 + 1));
					string v0_n = v0_str.substr(v0_split2 + 1, string::npos);
					//printf("%s, %s, %s\n", v0_v.c_str(), v0_t.c_str(), v0_n.c_str());
					string v1_v = v1_str.substr(0, v1_split1);
					string v1_t = v1_str.substr(v1_split1 + 1, v1_split2 - (v1_split1 + 1));
					string v1_n = v1_str.substr(v1_split2 + 1, string::npos);
					//printf("%s, %s, %s\n", v1_v.c_str(), v1_t.c_str(), v1_n.c_str());
					string v2_v = v2_str.substr(0, v2_split1);
					string v2_t = v2_str.substr(v2_split1 + 1, v2_split2 - (v2_split1 + 1));
					string v2_n = v2_str.substr(v2_split2 + 1, string::npos);
					//printf("%s, %s, %s\n", v2_v.c_str(), v2_t.c_str(), v2_n.c_str());
					Tri* t = new Tri();
					t->vertices[0] = vertices[stoi(v0_v) - 1];
					t->vertices[1] = vertices[stoi(v1_v) - 1];
					t->vertices[2] = vertices[stoi(v2_v) - 1];
					t->texcoords[0] = tex_coords[stoi(v0_t) - 1];
					t->texcoords[1] = tex_coords[stoi(v1_t) - 1];
					t->texcoords[2] = tex_coords[stoi(v2_t) - 1];
					t->normals[0] = normals[stoi(v0_n) - 1];
					t->normals[1] = normals[stoi(v1_n) - 1];
					t->normals[2] = normals[stoi(v2_n) - 1];
					triangles[tri_count] = t;
					tri_count += 1;
				}
				else 
				{
					// ToDo: Handle More Symbols (Multiple object files, Curves, Groups, Etc...)
					//printf(symbol.c_str());
				}
			}
		}
		else 
		{
			return 1;
		}
		return 0;
	}

	int ObjTools::LoadBMPTexture(string file) 
	{
		HRESULT res = TextureLoader.Load(file.c_str());
		return (int)res;
	}

	void* ObjTools::GetTexturePointer()
	{
		return TextureLoader.GetBits();
	}

	int ObjTools::GetTextureWidth()
	{
		return TextureLoader.GetWidth();
	}

	int ObjTools::GetTextureHeight()
	{
		return TextureLoader.GetHeight();
	}

	int ObjTools::GetTexturePitch() 
	{
		return TextureLoader.GetPitch();
	}

	int ObjTools::GetTextureBPP()
	{
		return TextureLoader.GetBPP();
	}
};