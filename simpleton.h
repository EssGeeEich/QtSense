#ifndef SIMPLETON_H
#define SIMPLETON_H
#include <memory>

template <typename T>
class Simpleton {
	std::unique_ptr<T> m_data;
public:
	T* operator -> () {
		if(!m_data)
			m_data.reset(new T);
		return m_data.get();
	}
};

#endif // SIMPLETON_H
