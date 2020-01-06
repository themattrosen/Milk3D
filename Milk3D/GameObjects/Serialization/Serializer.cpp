#include "Serializer.h"

namespace Milk3D
{

	Serializer::Serializer(const char * path, SerializeMode mode) :
		m_file(), m_mode(mode)
	{
		if (m_mode == sm_Save)
		{
			m_file.open(path, std::ios_base::out);
		}
		else
		{
			m_file.open(path, std::ios_base::in);
		}
	}

	Serializer::~Serializer()
	{
		m_file.close();
	}

	Serializer & Serializer::operator%(Vec3 & v)
	{
		switch (m_mode)
		{
		case sm_Save:
			m_file << v.X() << " " << v.Y() << " " << v.Z() << std::endl;
			break;
		case sm_Load:
			float x, y, z;
			m_file >> x >> y >> z;
			v.Set(x, y, z);
			break;
		}
		return *this;
	}

	Serializer & Serializer::operator%(Vec2 & v)
	{
		switch (m_mode)
		{
		case sm_Save:
			m_file << v.X() << " " << v.Y() << std::endl;
			break;
		case sm_Load:
			float x, y;
			m_file >> x >> y;
			v.Set(x, y);
			break;
		}
		return *this;
	}

	Serializer & Serializer::operator%(Vec4 & v)
	{
		switch (m_mode)
		{
		case sm_Save:
			m_file << v.X() << " " << v.Y() << " " << v.Z() << " " << v.W() << std::endl;
			break;
		case sm_Load:
			float x, y, z, w;
			m_file >> x >> y >> z >> w;
			v.Set(x, y, z, w);
			break;
		}
		return *this;
	}

	Serializer & Serializer::operator%(Quaternion & v)
	{
		switch (m_mode)
		{
		case sm_Save:
			m_file << v.X() << " " << v.Y() << " " << v.Z() << " " << v.W() << std::endl;
			break;
		case sm_Load:
			float x, y, z, w;
			m_file >> x >> y >> z >> w;
			v.AddRotation(Quaternion(x, y, z, w));
			break;
		}
		return *this;
	}

	Serializer & Serializer::operator%(Serializer::Endline & v)
	{
		switch (m_mode)
		{
		case sm_Save:
			m_file << std::endl;
			break;
		case sm_Load:
			
			break;
		}
		return *this;
	}

	Serializer & Serializer::operator%(Asset & a)
	{
		switch (m_mode)
		{
			case sm_Save:
				m_file << a.m_dir;
				m_file << a.m_name;
				m_file << (int)a.m_type;
				break;
			case sm_Load:
			{
				a.Unload();
				m_file >> a.m_dir;
				m_file >> a.m_name;
				int typeTemp;
				m_file >> typeTemp;
				a.m_type = (AssetType)typeTemp;

				a.Load();
				break;
			}
		}

		return *this;
	}

}