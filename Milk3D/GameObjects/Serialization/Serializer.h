#pragma once
#include <fstream>
#include <vector>

namespace Milk3D
{
	enum SerializeMode
	{
		sm_Save,
		sm_Load
	};

	class Serializer
	{
	public:
		Serializer(const char* path, SerializeMode mode);
		~Serializer();

		// primitive types
		template<typename T>
		Serializer& operator%(T& in)
		{
			switch (m_mode)
			{
			case sm_Save:
				m_file << in << " ";
				break;
			case sm_Load:
				m_file >> in;
				break;
			}
		}

		template<typename T>
		Serializer& operator%(std::vector<T>& vec)
		{
			switch (m_mode)
			{
				case sm_Save:
				{
					size_t size = vec.size();
					m_file << size << " ";
					for (size_t i = 0; i < size; ++i)
					{
						*this % vec[i];
					}
					break;
				}
				case sm_Load:
				{
					size_t size;
					m_file >> size;
					vec.clear();
					vec.reserve(size);
					for (size_t i = 0; i < size; ++i)
					{
						T next;
						*this % next;
						vec.push_back(next);
					}
					break;
				}
			}
		}

	private:
		std::fstream m_file;
		SerializeMode m_mode;
	};
}