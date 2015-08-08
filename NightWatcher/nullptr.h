#pragma once

namespace std {
	class nullptr_t {
	public:
		template<class T>
		operator T*() const {
			return 0;
		}

		template<class T, class U>
		operator T U::*() const {
			return 0;
		}

		operator bool( ) const {
			return false;
		}
	private:
		void* p;
		void operator&( ) { }
	};

	template<class T>
	bool operator==( T* lhs, const nullptr_t rhs ) {
		return lhs == 0;
	}

	template<class T>
	bool operator==( const nullptr_t lhs, T* rhs ) {
		return rhs == 0;
	}

	template<class T, class U>
	bool operator==( T U::* lhs, const nullptr_t rhs ) {
		return lhs == 0;
	}

	template<class T, class U>
	bool operator==( const nullptr_t lhs, T U::* rhs ) {
		return rhs == 0;
	}

	template<class T>
	bool operator!=( T* lhs, const nullptr_t rhs ) {
		return lhs != 0;
	}

	template<class T>
	bool operator!=( const nullptr_t lhs, T* rhs ) {
		return rhs != 0;
	}

	template<class T, class U>
	bool operator!=( T U::* lhs, const nullptr_t rhs ) {
		return lhs != 0;
	}

	template<class T, class U>
	bool operator!=( const nullptr_t lhs, T U::* rhs ) {
		return rhs != 0;
	}
}
const std::nullptr_t nullptr = std::nullptr_t( );

