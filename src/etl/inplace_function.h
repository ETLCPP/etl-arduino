#pragma once

#include "utility.h"
#include "optional.h"

#include <type_traits>
#include <new>

namespace etl
{
  //*************************************************************************
  /// VTable for inplace_function
  //*************************************************************************
  template <typename TReturn, typename... TArgs>
  struct inplace_function_vtable
  {
    using invoke_fn  = TReturn(*)(void*, TArgs...);
    using destroy_fn = void(*)(void*);
    using move_fn    = void(*)(void* dst, void* src);
    using copy_fn    = void(*)(void* dst, const void* src);

    invoke_fn  invoke_  = nullptr;
    destroy_fn destroy_ = nullptr;
    move_fn    move_    = nullptr;
    copy_fn    copy_    = nullptr;

    template <typename T>
    struct member_state
    {
      T* obj;
      TReturn (T::*member)(TArgs...);
    };

    template <typename T>
    struct const_member_state
    {
      const T* obj;
      TReturn (T::*member)(TArgs...) const;
    };

    //*******************************************
    // Create vtable for free function pointer
    //*******************************************
    static const inplace_function_vtable* for_function_ptr() 
    {
      using fn_t = TReturn(*)(TArgs...);

      static const inplace_function_vtable vtable
      {
        // invoke
        [](void* p, TArgs... a) -> TReturn
        {         
          auto* fn = static_cast<fn_t*>(p);
          return (*fn)(etl::forward<TArgs>(a)...);
        },
        // destroy
        nullptr,
        // move
        [](void* dst, void* src)
        {
          ::new (dst) fn_t(*static_cast<fn_t*>(src));
        },
        // copy
        [](void* dst, const void* src)
        {
          ::new (dst) fn_t(*static_cast<const fn_t*>(src));
        }
      };

      return &vtable;
    }

    //*******************************************
    // Create vtable for non-const member
    //*******************************************
    template <typename T>
    static const inplace_function_vtable* for_member() 
    {
      using state_t = member_state<T>;

      static const inplace_function_vtable vtable 
      {     
        // invoke
        [](void* p, TArgs... a) -> TReturn 
        {
          auto* s = static_cast<state_t*>(p);
          return (s->obj->*s->member)(etl::forward<TArgs>(a)...);
        },
        // destroy
        nullptr,
        // move
        [](void* dst, void* src) 
        {
          ::new (dst) state_t(etl::move(*static_cast<state_t*>(src)));
        },
        // copy
        [](void* dst, const void* src) 
        {
          ::new (dst) state_t(*static_cast<const state_t*>(src));
        }
      };

      return &vtable;
    }

    //*******************************************
    // Create vtable for const member
    //*******************************************
    template <typename T>
    static const inplace_function_vtable* for_const_member() 
    {   
      using state_t = const_member_state<T>;

      static const inplace_function_vtable vtable 
      {
        // invoke
        [](void* p, TArgs... a) -> TReturn 
        {
          auto* s = static_cast<state_t*>(p);
          return (s->obj->*s->member)(etl::forward<TArgs>(a)...);
        },
        // destroy
        nullptr,
        // move
        [](void* dst, void* src) 
        { 
          ::new (dst) state_t(etl::move(*static_cast<state_t*>(src))); 
        },
        // copy
        [](void* dst, const void* src) 
        { 
          ::new (dst) state_t(*static_cast<const state_t*>(src)); 
        }
      };

      return &vtable;
    }

    //*******************************************
    // Create vtable for functor/lambda T
    //*******************************************
    template <typename T>
    static const inplace_function_vtable* for_functor() 
    {
      // Choose destroy and move stubs based on trivial destructibility.
      destroy_fn destroy_ptr = etl::is_trivially_destructible<T>::value ? nullptr 
                                                                        : [](void* p) { static_cast<T*>(p)->~T(); };

      move_fn move_ptr = etl::is_trivially_destructible<T>::value ? [](void* dst, void* src) { ::new (dst) T(etl::move(*static_cast<T*>(src))); }
                                                                  : [](void* dst, void* src) { ::new (dst) T(etl::move(*static_cast<T*>(src))); static_cast<T*>(src)->~T(); };

      static const inplace_function_vtable vtable 
      {
        // invoke
        [](void* p, TArgs... a) -> TReturn { return (*static_cast<T*>(p))(etl::forward<TArgs>(a)...); },
        // destroy
        destroy_ptr,
        // move
        move_ptr,
        // copy
        [](void* dst, const void* src) { ::new (dst) T(*static_cast<const T*>(src)); }
      };

      return &vtable;
    }
  };

  //*************************************************************************
  template <typename TSignature, size_t Object_Size, size_t Object_Alignment>
  class inplace_function;

  //*************************************************************************
  // Is not an etl::inplace_function
  template <typename T>
  struct is_inplace_function : etl::false_type {};

  // Is an etl::inplace_function
  template <typename TReturn, typename... TArgs, size_t Object_Size, size_t Object_Alignment>
  struct is_inplace_function<etl::inplace_function<TReturn(TArgs...), Object_Size, Object_Alignment>> : etl::true_type {};

  //*************************************************************************
  /// inplace_function
  template <typename TReturn, typename... TArgs, size_t Object_Size, size_t Object_Alignment>
  class inplace_function<TReturn(TArgs...), Object_Size, Object_Alignment>
  {
    using this_type     = inplace_function<TReturn(TArgs...), Object_Size, Object_Alignment>;
    using function_type = TReturn(*)(TArgs...);

    using invoke_fn  = TReturn(*)(void*, TArgs...);
    using destroy_fn = void(*)(void*);
    using move_fn    = void(*)(void* dst, void* src);
    using copy_fn    = void(*)(void* dst, const void* src);

    using storage_type = etl::uninitialized_buffer<Object_Size, 1, Object_Alignment>;

    using vtable_type = inplace_function_vtable<TReturn, TArgs...>;

  public:

    //*************************************************************************
    inplace_function() noexcept = default;

    //*************************************************************************
    inplace_function(const inplace_function& other)
    {
      clone_from(other);
    }

    //*************************************************************************
    template <size_t Other_Object_Size, size_t Other_Object_Alignment>
    inplace_function(const etl::inplace_function<TReturn(TArgs...), Other_Object_Size, Other_Object_Alignment>& other)
    {
      static_assert(Object_Size      >= Other_Object_Size,      "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= Other_Object_Alignment, "etl::inplace_function: SBO alignment too small");

      clone_from(other);
    }

    //*************************************************************************
    inplace_function(inplace_function&& other) noexcept
    {
      move_from(other);
    }

    //*************************************************************************
    template <size_t Other_Object_Size, size_t Other_Object_Alignment>
    inplace_function(etl::inplace_function<TReturn(TArgs...), Other_Object_Size, Other_Object_Alignment>&& other) noexcept
    {
      static_assert(Object_Size      >= Other_Object_Size,      "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= Other_Object_Alignment, "etl::inplace_function: SBO alignment too small");

      move_from(other);
    }

    //*************************************************************************
    ~inplace_function()
    {
      clear();
    }

    //*************************************************************************
    inplace_function& operator =(const inplace_function& rhs)
    {
      if (this != &rhs)
      {
        clear();
        clone_from(rhs);
      }

      return *this;
    }

    //*************************************************************************
    inplace_function& operator =(inplace_function&& rhs) noexcept
    {
      if (this != &rhs)
      {
        clear();
        move_from(rhs);
      }

      return *this;
    }

    //*************************************************************************
    // Create from function pointer (runtime).
    // Stores the pointer into SBO and dispatches via stub.
    //*************************************************************************
    inplace_function(function_type f)
    {
      static_assert(Object_Size      >=sizeof(function_type),   "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= alignof(function_type), "etl::inplace_function: SBO alignment too small");

      auto* slot = reinterpret_cast<function_type*>(&storage_);
      ::new (slot) function_type(f);

      vtable = inplace_function_vtable<TReturn, TArgs...>::for_function_ptr();

      object_ = &storage_;
    }

    //*************************************************************************
    // Create from object + non-const member function (runtime).
    // Stores {object*, member ptr} in SBO and dispatches via stub.
    //*************************************************************************
    template <typename TObject>
    inplace_function(TObject& obj, TReturn (TObject::*method)(TArgs...))
    {
      using state_t = inplace_function_vtable<TReturn, TArgs...>::template member_state<TObject>;

      static_assert(Object_Size      >= sizeof(state_t),  "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= alignof(state_t), "etl::inplace_function: SBO alignment too small");

      auto* slot = reinterpret_cast<state_t*>(&storage_);
      ::new (slot) state_t{ &obj, method };

      object_ = &storage_;
      vtable  = inplace_function_vtable<TReturn, TArgs...>::template for_member<TObject>();
    }

    //*************************************************************************
    // Create from object + const member function (runtime).
    // Stores {object*, member ptr} in SBO and dispatches via stub.
    //*************************************************************************
    template <typename TObject>
    inplace_function(const TObject& obj, TReturn(TObject::* method)(TArgs...) const)
    {
      using state_t = inplace_function_vtable<TReturn, TArgs...>::template const_member_state<TObject>;

      static_assert(Object_Size      >= sizeof(state_t),  "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= alignof(state_t), "etl::inplace_function: SBO alignment too small");

      auto* slot = reinterpret_cast<state_t*>(&storage_);
      ::new (slot) state_t{ &obj, method };

      object_ = &storage_;
      vtable = inplace_function_vtable<TReturn, TArgs...>::template for_const_member<TObject>();
    }

    //*************************************************************************
    // Create from lambda/functor (runtime, SBO-owned).
    // Stores the callable into SBO and dispatches via a typed stub.
    //*************************************************************************
    template <typename TLambda,
              typename T = typename etl::decay<TLambda>::type,
              typename = etl::enable_if_t<etl::is_class<T>::value && !is_inplace_function<T>::value, void>>
      inplace_function(TLambda&& lambda)
    {
      static_assert(Object_Size      >=sizeof(T),   "etl::inplace_function: SBO size too small");
      static_assert(Object_Alignment >= alignof(T), "etl::inplace_function: SBO alignment too small");

      void* slot = &storage_;
      ::new (slot) T(etl::forward<TLambda>(lambda));

      object_ = &storage_;
      vtable = inplace_function_vtable<TReturn, TArgs...>::template for_functor<T>();
    }

    //*************************************************************************
    bool is_valid() const noexcept
    {
      return (vtable != nullptr);
    }

    //*************************************************************************
    explicit operator bool() const noexcept
    {
      return vtable != nullptr;
    }

    //*************************************************************************
    TReturn operator()(TArgs... args) const
    {
      // ETL_ASSERT(invoke_ != nullptr, ETL_ERROR(inplace_function_uninitialised));
      return vtable->invoke_(object_, etl::forward<TArgs>(args)...);
    }

    //*************************************************************************
    /// Execute the is_inplace_function if valid.
    /// 'void' return is_inplace_function.
    //*************************************************************************
    template <typename TRet = TReturn>
    typename etl::enable_if_t<etl::is_same<TRet, void>::value, bool>
      call_if(TArgs... args) const
    {
      if (is_valid())
      {
        vtable->invoke_(object_, etl::forward<TArgs>(args)...);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    /// Execute the is_inplace_function if valid.
    /// Non 'void' return is_inplace_function.
    //*************************************************************************
    template <typename TRet = TReturn>
    typename etl::enable_if_t<!etl::is_same<TRet, void>::value, etl::optional<TReturn>>
      call_if(TArgs... args) const
    {
      etl::optional<TReturn> result;

      if (is_valid())
      {
        result = vtable->invoke_(object_, etl::forward<TArgs>(args)...);
      }

      return result;
    }

    //*************************************************************************
    /// Execute the is_inplace_function if valid or call alternative.
    /// Run time alternative.
    //*************************************************************************
    template <typename TAlternative>
    ETL_CONSTEXPR14 TReturn call_or(TAlternative&& alternative, TArgs... args) const
    {
      if (is_valid())
      {
        return vtable->invoke_(object_, etl::forward<TArgs>(args)...);
      }
      else
      {
        return etl::forward<TAlternative>(alternative)(etl::forward<TArgs>(args)...);
      }
    }

    //*************************************************************************
    /// Execute the is_inplace_function if valid or call alternative.
    /// Compile time alternative.
    //*************************************************************************
    template <TReturn(*Method)(TArgs...)>
    ETL_CONSTEXPR14 TReturn call_or(TArgs... args) const
    {
      if (is_valid())
      {
        return vtable->invoke_(object_, etl::forward<TArgs>(args)...);
      }
      else
      {
        return (Method)(etl::forward<TArgs>(args)...);
      }
    }

  private:

    ////*************************************************************************
    // clear
    ////*************************************************************************
    void clear() noexcept 
    {
      if (vtable && vtable->destroy_) 
      { 
        vtable->destroy_(object_); 
      }

      vtable = nullptr; 
      object_ = nullptr;
    }

    // Allow cross-size access to internals
    template <typename, size_t, size_t>
    friend class inplace_function;

    ////*************************************************************************
    // clone_from
    ////*************************************************************************
    template <size_t Object_Size, size_t Object_Alignment>
    void clone_from(const etl::inplace_function<TReturn(TArgs...), Object_Size, Object_Alignment>& other) 
    {
      vtable = other.vtable;

      if (vtable && vtable->copy_) 
      { 
        vtable->copy_(&storage_, &other.storage_); 
        object_ = &storage_; 
      }
      else 
      { 
        object_ = other.object_; 
      }
    }

    ////*************************************************************************
    // move_from
    ////*************************************************************************
    template <size_t Object_Size, size_t Object_Alignment>
    void move_from(etl::inplace_function<TReturn(TArgs...), Object_Size, Object_Alignment>& other) 
    {
      vtable = other.vtable;

      if (vtable && vtable->move_) 
      { 
        vtable->move_(&storage_, &other.storage_); 
        object_ = &storage_; 

        other.vtable  = nullptr;
        other.object_ = nullptr;
      }
      else 
      { 
        object_ = other.object_; 
        other.vtable  = nullptr; 
        other.object_ = nullptr;
      }
    }

    void*        object_ = nullptr;
    const vtable_type* vtable = nullptr;
    storage_type storage_;
  };
}