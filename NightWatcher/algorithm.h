#pragma once

namespace daw {
	template<class ForwardIt, class T>
	void fill( ForwardIt first, ForwardIt last, const T& value ) {
		for( ; first != last; ++first ) {
			*first = value;
		}
	}

	template<class ForwardIt, class T>
	void fill( ForwardIt first, ForwardIt last, const volatile T& value ) {
		for( ; first != last; ++first ) {
			*first = value;
		}
	}

	template<class InputIterator, class OutputIterator>
	OutputIterator copy( InputIterator first, InputIterator last, OutputIterator result ) {
		while( first != last ) {
			*result = *first;
			++result;
			++first;
		}
		return result;
	}
}
