/*
 * SmartPtr.h
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */

#pragma once

#include <new>
#include "Libraries/Std/AtomicOps.h"


namespace SmartPtrPolicy
{
	enum StoragePolicy {
		ArrayStorage,			   /**< Call 'delete []' for pointer */
		DoNotReleasePointee,		/**< Do not release pointer */
	};

	enum FailMethod {
		FailWithException,		  /**< Throw exception on failure */
		FailByValue,				/**< Became non-valid to indicate failure */
	};
}


namespace SmartPtrStorage
{
	// If your object class is based publicly on Refcounted, then
	// SmartPtr will not allocate additional storage to manage your object.
	//
	// The FailMethod and StoragePolicy policies will not be applicable in this case.
	// A compile-time error will raise when you try to create a SmartPtr with a policy for such objects.
	//
	// retain()/release() methods will be inaccessible with SmartPtr::operator->()
	// to prevent accidental retain/release of the managed object.

	class Refcounted
	{
	protected:
		Refcounted()
			: m_ref(1)
		{}
		virtual ~Refcounted() {}
		virtual void _destroy() {delete this;}

	public:
		void retain() {AtomicInc(&m_ref);}
		void release()
		{
			unsigned count = AtomicDec(&m_ref) - 1;
			if (count == 0)
				_destroy();
		}
		unsigned countRefs() const {return m_ref;}

	private:
		unsigned m_ref;
	};
}


namespace SmartPtrPrivate
{
	template <typename T>
	class Storage: public SmartPtrStorage::Refcounted
	{
		Storage(const Storage &);
		Storage &operator = (const Storage &);
	protected:
		Storage(T *p)
			: m_pointee(p)
		{}

	public:
		T *pointee() const {return m_pointee;}

	protected:
		T *m_pointee;
	};


	template <typename T>
	class StorageSingle: public Storage<T>
	{
		~StorageSingle() {delete Storage<T>::m_pointee;}
	public:
		StorageSingle(T *p): Storage<T>(p) {}
	};

	template <typename T>
	class StorageArray: public Storage<T>
	{
		~StorageArray() {delete[] Storage<T>::m_pointee;}
	public:
		StorageArray(T *p): Storage<T>(p) {}
	};

	template <typename T>
	class StorageNoRelease: public Storage<T>
	{
	public:
		StorageNoRelease(T *p): Storage<T>(p) {}
	};

	template <typename T, typename Free>
	class StorageWithFree: public Storage<T>
	{
		~StorageWithFree() {m_free(Storage<T>::m_pointee);}
	public:
		StorageWithFree(T *p, Free freeOp)
			: Storage<T>(p)
			, m_free(freeOp)
		{}
	private:
		Free m_free;
	};


	template <typename T>
		class StorageFactoryGeneric
		{
		public:
			typedef Storage<T> StorageType;
			typedef T *LimitedType;	// returned by operator->
			typedef void ResetType;	// returned by reset()

			static inline T *getPointee(StorageType *s)
			{
				return s? s->pointee(): 0;
			}


			static inline  LimitedType getLimitedPointee(StorageType *s)
			{
				return s? s->pointee(): 0;
			}

			static StorageType *allocSingleStorage(T *pointee)
			{
				if (StorageType *s = new(std::nothrow) StorageSingle<T>(pointee))
					return s;
				delete pointee;
				return 0;
			}

			static StorageType *allocSingleStorage(T *pointee, SmartPtrPolicy::FailMethod failMethod)
			{
				try
				{
					return new StorageSingle<T>(pointee);
				}
				catch (const std::bad_alloc &)
				{
					delete pointee;
					if (SmartPtrPolicy::FailWithException == failMethod)
						throw;
				}
				return 0;
			}

			template <typename Free>
			static StorageType *allocStorageWithFree(T *pointee, Free freeOp, SmartPtrPolicy::FailMethod failMethod)
			{
				try
				{
					return new StorageWithFree<T, Free>(pointee, freeOp);
				}
				catch (const std::bad_alloc &)
				{
					freeOp(pointee);
					if (SmartPtrPolicy::FailWithException == failMethod)
						throw;
				}
				return 0;
			}

			static StorageType *allocStorageWithPolicy(T *pointee,
					SmartPtrPolicy::FailMethod failMethod, SmartPtrPolicy::StoragePolicy policy)
			{
				try
				{
					switch (policy)
					{
					case SmartPtrPolicy::ArrayStorage:
						return new StorageArray<T>(pointee);
					case SmartPtrPolicy::DoNotReleasePointee:
						return new StorageNoRelease<T>(pointee);
					}
				}
				catch (const std::bad_alloc &)
				{
					if (SmartPtrPolicy::ArrayStorage == policy)
						delete[] pointee;
					if (SmartPtrPolicy::FailWithException == failMethod)
						throw;
				}
				return 0;
			}

			static inline ResetType storagePtr(StorageType **) {}
		};

	template <typename T>
		class StorageFactoryRefcounted
		{
		public:
			typedef T StorageType;
			typedef class NoRetainRelease: public T
			{
				~NoRetainRelease();
				void retain();
				void release();
			} *LimitedType;
			typedef T **ResetType;	// returned by reset()

			static inline T *getPointee(StorageType *s) {return s;}
			static inline LimitedType getLimitedPointee(StorageType *s) {return static_cast<LimitedType>(s);}
			static inline StorageType *allocSingleStorage(T *pointee) {return pointee;}
			static inline StorageType *allocSingleStorageAndRetain(T *pointee, bool retain)
			{
				if (retain)
					pointee->retain();
				return pointee;
			}
			static inline ResetType storagePtr(StorageType **s) {return s;}
		};

	namespace
	{
		// Empty template just for specialization
		template <unsigned, typename T>
			struct StorFactoryTypeHolder
			{
			};

		template <unsigned size>
			struct SizeIs
			{
				char _[size];
			};

		enum {
			StorageTypeInvalid,
			StorageTypeGeneric,
			StorageTypeRefcounted
		};

		template <typename T>
			struct StorFactoryTypeHolder<StorageTypeGeneric, T>
			{
				typedef StorageFactoryGeneric<T> Type;
			};
		template <typename T>
			struct StorFactoryTypeHolder<StorageTypeRefcounted, T>
			{
				typedef StorageFactoryRefcounted<T> Type;
			};

		struct SmartPtrSelectStorageByObjectType
		{
			static SizeIs<StorageTypeGeneric> overloadedFunc(const void *);
			static SizeIs<StorageTypeRefcounted> overloadedFunc(const SmartPtrStorage::Refcounted *);
		};
	}


	template <typename T>
		class StorageSelector
		{
			enum {
				index = sizeof(SmartPtrSelectStorageByObjectType::overloadedFunc((T *)0)),
			};

		public:
			typedef typename StorFactoryTypeHolder<index, T>::Type Type;
		};
}


template <class T>
class SmartPtr
{
	typedef typename SmartPtrPrivate::StorageSelector<T>::Type Delegate;
	typedef typename Delegate::StorageType StorageType;

public:
	typedef T *StoredType;	   // the type of the pointee object
	typedef T &ReferenceType;	// type returned by operator*
	typedef typename Delegate::LimitedType LimitedType;	  // type returned by operator->
	typedef typename Delegate::ResetType ResetType;	  // type returned by reset()

public:
	SmartPtr()
		: m_storage(0)
	{}

	explicit SmartPtr(StoredType pointee)
		: m_storage(Delegate::allocSingleStorage(pointee))
	{}

	SmartPtr(StoredType pointee, bool retain)
		: m_storage(Delegate::allocSingleStorageAndRetain(pointee, retain))
	{}

	SmartPtr(StoredType pointee, SmartPtrPolicy::FailMethod failMethod)
		: m_storage(Delegate::allocSingleStorage(pointee, failMethod))
	{}

	SmartPtr(StoredType pointee,
			SmartPtrPolicy::StoragePolicy storagePolicy,
			SmartPtrPolicy::FailMethod failMethod = SmartPtrPolicy::FailByValue)
		: m_storage(Delegate::allocStorageWithPolicy(pointee, failMethod, storagePolicy))
	{}

	template <typename Free>
	SmartPtr(StoredType pointee, Free freeOp,
			SmartPtrPolicy::FailMethod failMethod = SmartPtrPolicy::FailByValue)
		: m_storage(Delegate::allocStorageWithFree(pointee, freeOp, failMethod))
	{}

	SmartPtr(const SmartPtr &copy)
		: m_storage(copy.m_storage)
	{
		if (m_storage)
			m_storage->retain();
	}

	template <typename U>
	SmartPtr(const SmartPtr<U> &copy)
		: m_storage(copy.get())
	{
		if (m_storage)
			m_storage->retain();
	}

	~SmartPtr()
	{
		if (m_storage)
			m_storage->release();
	}

	const SmartPtr &operator = (const SmartPtr &copy)
	{
		SmartPtr(copy).swap(*this);
		return *this;
	}

	template <typename U>
	const SmartPtr &operator = (const SmartPtr<U> &copy)
	{
		SmartPtr(copy).swap(*this);
		return *this;
	}

	void swap(SmartPtr &rhs)
	{
		StorageType *tmp = rhs.m_storage;
		rhs.m_storage = m_storage;
		m_storage = tmp;
	}

	bool operator == (const SmartPtr &ptr) const
	{return m_storage == ptr.m_storage;}

	bool operator != (const SmartPtr &ptr) const
	{return !operator==(ptr);}

	bool operator < (const SmartPtr &ptr) const
	{return m_storage < ptr.m_storage;}

	StoredType get() const
	{
		return Delegate::getPointee(m_storage);
	}

	StoredType getImpl() const
	{
		return get();
	}

	LimitedType operator->() const
	{
		return Delegate::getLimitedPointee(m_storage);
	}

	ReferenceType operator*() const
	{
		return *get();
	}

	unsigned countRefs() const
	{
		return 0 != m_storage? m_storage->countRefs(): 0;
	}

	// return value type is void for Generic storage
	ResetType reset()
	{
		SmartPtr().swap(*this);
		return Delegate::storagePtr(&m_storage);
	}

	void reset(StoredType pointee)
	{
		SmartPtr(pointee).swap(*this);
	}

	void reset(StoredType pointee, SmartPtrPolicy::FailMethod failMethod)
	{
		SmartPtr(pointee, failMethod).swap(*this);
	}

	StoredType release()
	{
		// Will raise compile-time error for Generic storage due to
		// incompatible StoredType and StorageType *, which is Ok
		// (we don't allow release() for generic storage)
		StoredType p = m_storage;
		m_storage = 0;
		return p;
	}

	bool isValid() const
	{return 0 != get();}

	operator bool() const
	{return isValid();}

private:
	StorageType *m_storage;
};
