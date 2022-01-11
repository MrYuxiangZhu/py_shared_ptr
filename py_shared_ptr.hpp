#pragma once
#include <iostream>
#include <algorithm>
#include <Python.h>
#include <assert.h>

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

#ifndef _STD
#define _STD std::
#endif

namespace Python
{
class _Py_ref_count_base
{
public:
	constexpr _Py_ref_count_base() noexcept = default;

	_Py_ref_count_base(nullptr_t) noexcept : _Count(nullptr) { }

	_Py_ref_count_base(const _Py_ref_count_base& _Other) noexcept
	{
		this->_Count = _Other._Count;
	}

	~_Py_ref_count_base() noexcept = default;

	_Py_ref_count_base& operator=(nullptr_t) noexcept
	{
		this->_Count = nullptr;
		return *this;
	}

	_NODISCARD long _Use_Count() const noexcept
	{
		return this->_Count ? (*this->_Count) : 0;
	}

	template <typename _Ty>
	void _Incref(_Ty* _Ptr) noexcept
	{
		if (_Ptr)
		{
			if (nullptr == this->_Count)
			{
				this->_Count = new long(1);
			}
			else
			{
				++(*this->_Count);
			}
		}
	}

	template <typename _Ty>
	void _Decref(_Ty* _Ptr) noexcept
	{
		if (nullptr != this->_Count)
		{
			--(*this->_Count);
			if (0 == *this->_Count)
			{
				if (NULL != _Ptr && Py_REFCNT(_Ptr) > 0)
				{
					Py_XDECREF(_Ptr);
				}
				delete this->_Count;
				this->_Count = nullptr;
			}
		}
	}

private:
	long* _Count{ nullptr };
};

template <typename _Ty>
class _Py_ptr_base
{
protected:
	using _Element_type = _Ty;
public:
	_Py_ptr_base(const _Py_ptr_base&) = delete;
	_Py_ptr_base& operator=(const _Py_ptr_base&) = delete;

	_NODISCARD long use_count() const noexcept
	{
		return _Rep._Use_Count();
	}

	_NODISCARD _Element_type* get() const noexcept
	{
		return _Ptr;
	}

	_NODISCARD bool unique() const noexcept
	{
		return 1 == _Rep._Use_Count();
	}

	_NODISCARD _Element_type* operator->() const noexcept
	{
		return _Ptr;
	}

	_NODISCARD _Element_type& operator*() const noexcept
	{
		return *_Ptr;
	}

protected:
	constexpr _Py_ptr_base() noexcept = default;

	virtual ~_Py_ptr_base() noexcept = default;

	void _Copy_pointer_from(const _Ty* _Other) noexcept
	{
		this->_Ptr = const_cast<_Ty*>(_Other);
		this->_Incref();
	}

	template <typename _Uy, std::enable_if_t<std::is_convertible_v<_Uy*, _Ty*>, bool> = false>
	void _Move_construct_from(_Py_ptr_base<_Uy>&& _Right) noexcept
	{
		this->_Ptr = _Right._Ptr;
		this->_Rep = _Right._Rep;
		_Right._Ptr = nullptr;
		_Right._Rep = nullptr;
	}

	template <typename _Uy, std::enable_if_t<std::is_convertible_v<_Uy*, _Ty*>, bool> = false>
	void _Copy_construct_from(const _Py_ptr_base<_Uy>& _Other) noexcept
	{
		this->_Ptr = _Other._Ptr;
		this->_Rep = _Other._Rep;
		this->_Incref();
	}

	template <typename _Uy, std::enable_if_t<std::is_convertible_v<_Uy*, _Ty*>, bool> = false>
	void _Alias_construct_from(const _Py_ptr_base<_Uy>& _Other, _Ty* _Px) noexcept
	{
		this->_Ptr = _Px;
		this->_Rep = _Other._Rep;
		this->_Incref();
	}

	template <typename _Uy, std::enable_if_t<std::is_convertible_v<_Uy*, _Ty*>, bool> = false>
	void _Alias_move_construct_from(_Py_ptr_base<_Uy>&& _Right, _Ty* _Px) noexcept
	{
		this->_Ptr = _Px;
		this->_Rep = _Right._Rep;
		_Right._Ptr = nullptr;
		_Right._Rep = nullptr;
	}

	void _Swap(_Py_ptr_base& _Right) noexcept
	{
		std::swap(this->_Ptr, _Right._Ptr);
		std::swap(this->_Rep, _Right._Rep);
	}

	void _Incref() noexcept
	{
		_Rep._Incref(this->_Ptr);
	}

	void _Decref() noexcept
	{
		_Rep._Decref(this->_Ptr);
	}

protected:
	_Element_type* _Ptr{ nullptr };
	_Py_ref_count_base _Rep{ nullptr };
};

template <typename _Ty>
class py_shared_ptr : public _Py_ptr_base<_Ty>
{
private:
	using _Shared_base = _Py_ptr_base<_Ty>;
public:
	using typename _Shared_base::_Element_type;

	constexpr py_shared_ptr() noexcept = default;

	constexpr py_shared_ptr(nullptr_t) noexcept { }

	py_shared_ptr(_Element_type* _Px) noexcept
	{
		__super::_Copy_pointer_from(_Px);
	}

	py_shared_ptr(const py_shared_ptr& _Other) noexcept
	{
		__super::_Copy_construct_from(_Other);
	}

	py_shared_ptr(py_shared_ptr&& _Right) noexcept
	{
		__super::_Move_construct_from(std::move(_Right));
	}

	template <typename _Uy>
	py_shared_ptr(const py_shared_ptr<_Uy>& _Other) noexcept
	{
		__super::_Copy_construct_from(_Other);
	}

	template <typename _Uy>
	py_shared_ptr(py_shared_ptr<_Uy>&& _Right) noexcept
	{
		__super::_Alias_move_construct_from(std::move(_Right));
	}

	template <typename _Uy>
	py_shared_ptr(const py_shared_ptr<_Uy>& _Other, _Element_type* _Ptr)
	{
		__super::_Alias_construct_from(_Other, _Ptr);
	}

	template <typename _Uy>
	py_shared_ptr(const py_shared_ptr<_Uy>&& _Right, _Element_type* _Ptr) noexcept
	{
		__super::_Alias_move_construct_from(std::move(_Right), _Ptr);
	}

	~py_shared_ptr() noexcept
	{
		__super::_Decref();
	}

	py_shared_ptr& operator=(_Element_type* _Other) noexcept
	{
		py_shared_ptr(_Other).swap(*this);
		return *this;
	}

	py_shared_ptr& operator=(const py_shared_ptr& _Other) noexcept
	{
		py_shared_ptr(_Other).swap(*this);
		return *this;
	}

	py_shared_ptr& operator=(py_shared_ptr&& _Right) noexcept
	{
		py_shared_ptr(std::move(_Right)).swap(*this);
		return *this;
	}

	template <typename _Uy>
	py_shared_ptr& operator=(const py_shared_ptr<_Uy>& _Other) noexcept
	{
		py_shared_ptr(_Other).swap(*this);
		return *this;
	}

	template <typename _Uy>
	py_shared_ptr& operator=(py_shared_ptr<_Uy>&& _Right) noexcept
	{
		py_shared_ptr(std::move(_Right)).swap(*this);
		return *this;
	}

	void swap(py_shared_ptr& _Other) noexcept
	{
		__super::_Swap(_Other);
	}

	void reset() noexcept //release resource and convert to empty shared_ptr
	{
		py_shared_ptr().swap(*this);
	}

	template <typename _Uy>
	void reset(_Uy* _Ptr) noexcept
	{
		py_shared_ptr(_Ptr).swap(*this);
	}
};

template <typename _Ty1, typename _Ty2>
_NODISCARD bool operator==(const py_shared_ptr<_Ty1>& _Left, const py_shared_ptr<_Ty2>& _Right) noexcept
{
	return _Left.get() == _Right.get();
}

template <typename _Ty1, typename _Ty2>
_NODISCARD bool operator<(const py_shared_ptr<_Ty1>& _Left, const py_shared_ptr<_Ty2>& _Right) noexcept
{
	return _Left.get() < _Right.get();
}

template <typename _Ty1, typename _Ty2>
_NODISCARD bool operator<=(const py_shared_ptr<_Ty1>& _Left, const py_shared_ptr<_Ty2>& _Right) noexcept
{
	return _Left.get() <= _Right.get();
}

template <typename _Ty1, typename _Ty2>
_NODISCARD bool operator>(const py_shared_ptr<_Ty1>& _Left, const py_shared_ptr<_Ty2>& _Right) noexcept
{
	return _Left.get() > _Right.get();
}

template <typename _Ty1, typename _Ty2>
_NODISCARD bool operator>=(const py_shared_ptr<_Ty1>& _Left, const py_shared_ptr<_Ty2>& _Right) noexcept
{
	return _Left.get() >= _Right.get();
}

template <typename _Ty>
_NODISCARD bool operator==(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() == nullptr;
}

template <typename _Ty>
_NODISCARD bool operator==(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return nullptr == _Right.get();
}

template <class _Ty>
_NODISCARD bool operator!=(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() != nullptr;
}

template <class _Ty>
_NODISCARD bool operator!=(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return nullptr != _Right.get();
}

template <class _Ty>
_NODISCARD bool operator<(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() < static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr);
}

template <class _Ty>
_NODISCARD bool operator<(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr) < _Right.get();
}

template <class _Ty>
_NODISCARD bool operator>=(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() >= static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr);
}

template <class _Ty>
_NODISCARD bool operator>=(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr) >= _Right.get();
}

template <class _Ty>
_NODISCARD bool operator>(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() > static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr);
}

template <class _Ty>
_NODISCARD bool operator>(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr) > _Right.get();
}

template <class _Ty>
_NODISCARD bool operator<=(const py_shared_ptr<_Ty>& _Left, nullptr_t) noexcept
{
	return _Left.get() <= static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr);
}

template <class _Ty>
_NODISCARD bool operator<=(nullptr_t, const py_shared_ptr<_Ty>& _Right) noexcept
{
	return static_cast<typename py_shared_ptr<_Ty>::_Element_type*>(nullptr) <= _Right.get();
}

template <class _Ty>
void swap(py_shared_ptr<_Ty>& _Left, py_shared_ptr<_Ty>& _Right) noexcept
{
	_Left.swap(_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> static_pointer_cast(const py_shared_ptr<_Ty2>& _Other) noexcept
{
	const auto _Ptr = static_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_Other, _Ptr);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> static_pointer_cast(py_shared_ptr<_Ty2>&& _Other) noexcept
{
	const auto _Ptr = static_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_STD move(_Other), _Ptr);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> const_pointer_cast(const py_shared_ptr<_Ty2>& _Other) noexcept
{
	const auto _Ptr = const_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_Other, _Ptr);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> const_pointer_cast(py_shared_ptr<_Ty2>&& _Other) noexcept
{
	const auto _Ptr = const_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_STD move(_Other), _Ptr);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> reinterpret_pointer_cast(const py_shared_ptr<_Ty2>& _Other) noexcept
{
	const auto _Ptr = reinterpret_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_Other, _Ptr);
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> reinterpret_pointer_cast(py_shared_ptr<_Ty2>&& _Other) noexcept
{
	const auto _Ptr = reinterpret_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());
	return py_shared_ptr<_Ty1>(_STD move(_Other), _Ptr);
}

#ifdef _CPPRTTI
template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> dynamic_pointer_cast(const py_shared_ptr<_Ty2>& _Other) noexcept
{
	const auto _Ptr = dynamic_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());

	if (_Ptr)
	{
		return py_shared_ptr<_Ty1>(_Other, _Ptr);
	}

	return {};
}

template <class _Ty1, class _Ty2>
_NODISCARD py_shared_ptr<_Ty1> dynamic_pointer_cast(py_shared_ptr<_Ty2>&& _Other) noexcept
{
	const auto _Ptr = dynamic_cast<typename py_shared_ptr<_Ty1>::_Element_type*>(_Other.get());

	if (_Ptr)
	{
		return py_shared_ptr<_Ty1>(_STD move(_Other), _Ptr);
	}

	return {};
}
#else // _CPPRTTI
template <class _Ty1, class _Ty2>
py_shared_ptr<_Ty1> dynamic_pointer_cast(const py_shared_ptr<_Ty2>&) noexcept = delete; // requires /GR option
template <class _Ty1, class _Ty2>
py_shared_ptr<_Ty1> dynamic_pointer_cast(py_shared_ptr<_Ty2>&&) noexcept = delete; // requires /GR option
#endif // _CPPRTTI

}
