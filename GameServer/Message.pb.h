// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Message.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Message_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Message_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "Enum.pb.h"
#include "Struct.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Message_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Message_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Message_2eproto;
namespace ProjectJ {
class C_VERIFY_TOKEN;
struct C_VERIFY_TOKENDefaultTypeInternal;
extern C_VERIFY_TOKENDefaultTypeInternal _C_VERIFY_TOKEN_default_instance_;
class S_VERIFY_TOKEN;
struct S_VERIFY_TOKENDefaultTypeInternal;
extern S_VERIFY_TOKENDefaultTypeInternal _S_VERIFY_TOKEN_default_instance_;
}  // namespace ProjectJ
PROTOBUF_NAMESPACE_OPEN
template<> ::ProjectJ::C_VERIFY_TOKEN* Arena::CreateMaybeMessage<::ProjectJ::C_VERIFY_TOKEN>(Arena*);
template<> ::ProjectJ::S_VERIFY_TOKEN* Arena::CreateMaybeMessage<::ProjectJ::S_VERIFY_TOKEN>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace ProjectJ {

// ===================================================================

class C_VERIFY_TOKEN final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProjectJ.C_VERIFY_TOKEN) */ {
 public:
  inline C_VERIFY_TOKEN() : C_VERIFY_TOKEN(nullptr) {}
  ~C_VERIFY_TOKEN() override;
  explicit PROTOBUF_CONSTEXPR C_VERIFY_TOKEN(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  C_VERIFY_TOKEN(const C_VERIFY_TOKEN& from);
  C_VERIFY_TOKEN(C_VERIFY_TOKEN&& from) noexcept
    : C_VERIFY_TOKEN() {
    *this = ::std::move(from);
  }

  inline C_VERIFY_TOKEN& operator=(const C_VERIFY_TOKEN& from) {
    CopyFrom(from);
    return *this;
  }
  inline C_VERIFY_TOKEN& operator=(C_VERIFY_TOKEN&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const C_VERIFY_TOKEN& default_instance() {
    return *internal_default_instance();
  }
  static inline const C_VERIFY_TOKEN* internal_default_instance() {
    return reinterpret_cast<const C_VERIFY_TOKEN*>(
               &_C_VERIFY_TOKEN_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(C_VERIFY_TOKEN& a, C_VERIFY_TOKEN& b) {
    a.Swap(&b);
  }
  inline void Swap(C_VERIFY_TOKEN* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(C_VERIFY_TOKEN* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  C_VERIFY_TOKEN* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<C_VERIFY_TOKEN>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const C_VERIFY_TOKEN& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const C_VERIFY_TOKEN& from) {
    C_VERIFY_TOKEN::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(C_VERIFY_TOKEN* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProjectJ.C_VERIFY_TOKEN";
  }
  protected:
  explicit C_VERIFY_TOKEN(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTokenFieldNumber = 2,
    kAccountIdFieldNumber = 1,
  };
  // string token = 2;
  void clear_token();
  const std::string& token() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_token(ArgT0&& arg0, ArgT... args);
  std::string* mutable_token();
  PROTOBUF_NODISCARD std::string* release_token();
  void set_allocated_token(std::string* token);
  private:
  const std::string& _internal_token() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_token(const std::string& value);
  std::string* _internal_mutable_token();
  public:

  // int32 account_id = 1;
  void clear_account_id();
  int32_t account_id() const;
  void set_account_id(int32_t value);
  private:
  int32_t _internal_account_id() const;
  void _internal_set_account_id(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:ProjectJ.C_VERIFY_TOKEN)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr token_;
    int32_t account_id_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_Message_2eproto;
};
// -------------------------------------------------------------------

class S_VERIFY_TOKEN final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProjectJ.S_VERIFY_TOKEN) */ {
 public:
  inline S_VERIFY_TOKEN() : S_VERIFY_TOKEN(nullptr) {}
  ~S_VERIFY_TOKEN() override;
  explicit PROTOBUF_CONSTEXPR S_VERIFY_TOKEN(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  S_VERIFY_TOKEN(const S_VERIFY_TOKEN& from);
  S_VERIFY_TOKEN(S_VERIFY_TOKEN&& from) noexcept
    : S_VERIFY_TOKEN() {
    *this = ::std::move(from);
  }

  inline S_VERIFY_TOKEN& operator=(const S_VERIFY_TOKEN& from) {
    CopyFrom(from);
    return *this;
  }
  inline S_VERIFY_TOKEN& operator=(S_VERIFY_TOKEN&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const S_VERIFY_TOKEN& default_instance() {
    return *internal_default_instance();
  }
  static inline const S_VERIFY_TOKEN* internal_default_instance() {
    return reinterpret_cast<const S_VERIFY_TOKEN*>(
               &_S_VERIFY_TOKEN_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(S_VERIFY_TOKEN& a, S_VERIFY_TOKEN& b) {
    a.Swap(&b);
  }
  inline void Swap(S_VERIFY_TOKEN* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(S_VERIFY_TOKEN* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  S_VERIFY_TOKEN* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<S_VERIFY_TOKEN>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const S_VERIFY_TOKEN& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const S_VERIFY_TOKEN& from) {
    S_VERIFY_TOKEN::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(S_VERIFY_TOKEN* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProjectJ.S_VERIFY_TOKEN";
  }
  protected:
  explicit S_VERIFY_TOKEN(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kResultFieldNumber = 1,
  };
  // bool result = 1;
  void clear_result();
  bool result() const;
  void set_result(bool value);
  private:
  bool _internal_result() const;
  void _internal_set_result(bool value);
  public:

  // @@protoc_insertion_point(class_scope:ProjectJ.S_VERIFY_TOKEN)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    bool result_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_Message_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// C_VERIFY_TOKEN

// int32 account_id = 1;
inline void C_VERIFY_TOKEN::clear_account_id() {
  _impl_.account_id_ = 0;
}
inline int32_t C_VERIFY_TOKEN::_internal_account_id() const {
  return _impl_.account_id_;
}
inline int32_t C_VERIFY_TOKEN::account_id() const {
  // @@protoc_insertion_point(field_get:ProjectJ.C_VERIFY_TOKEN.account_id)
  return _internal_account_id();
}
inline void C_VERIFY_TOKEN::_internal_set_account_id(int32_t value) {
  
  _impl_.account_id_ = value;
}
inline void C_VERIFY_TOKEN::set_account_id(int32_t value) {
  _internal_set_account_id(value);
  // @@protoc_insertion_point(field_set:ProjectJ.C_VERIFY_TOKEN.account_id)
}

// string token = 2;
inline void C_VERIFY_TOKEN::clear_token() {
  _impl_.token_.ClearToEmpty();
}
inline const std::string& C_VERIFY_TOKEN::token() const {
  // @@protoc_insertion_point(field_get:ProjectJ.C_VERIFY_TOKEN.token)
  return _internal_token();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void C_VERIFY_TOKEN::set_token(ArgT0&& arg0, ArgT... args) {
 
 _impl_.token_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:ProjectJ.C_VERIFY_TOKEN.token)
}
inline std::string* C_VERIFY_TOKEN::mutable_token() {
  std::string* _s = _internal_mutable_token();
  // @@protoc_insertion_point(field_mutable:ProjectJ.C_VERIFY_TOKEN.token)
  return _s;
}
inline const std::string& C_VERIFY_TOKEN::_internal_token() const {
  return _impl_.token_.Get();
}
inline void C_VERIFY_TOKEN::_internal_set_token(const std::string& value) {
  
  _impl_.token_.Set(value, GetArenaForAllocation());
}
inline std::string* C_VERIFY_TOKEN::_internal_mutable_token() {
  
  return _impl_.token_.Mutable(GetArenaForAllocation());
}
inline std::string* C_VERIFY_TOKEN::release_token() {
  // @@protoc_insertion_point(field_release:ProjectJ.C_VERIFY_TOKEN.token)
  return _impl_.token_.Release();
}
inline void C_VERIFY_TOKEN::set_allocated_token(std::string* token) {
  if (token != nullptr) {
    
  } else {
    
  }
  _impl_.token_.SetAllocated(token, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.token_.IsDefault()) {
    _impl_.token_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:ProjectJ.C_VERIFY_TOKEN.token)
}

// -------------------------------------------------------------------

// S_VERIFY_TOKEN

// bool result = 1;
inline void S_VERIFY_TOKEN::clear_result() {
  _impl_.result_ = false;
}
inline bool S_VERIFY_TOKEN::_internal_result() const {
  return _impl_.result_;
}
inline bool S_VERIFY_TOKEN::result() const {
  // @@protoc_insertion_point(field_get:ProjectJ.S_VERIFY_TOKEN.result)
  return _internal_result();
}
inline void S_VERIFY_TOKEN::_internal_set_result(bool value) {
  
  _impl_.result_ = value;
}
inline void S_VERIFY_TOKEN::set_result(bool value) {
  _internal_set_result(value);
  // @@protoc_insertion_point(field_set:ProjectJ.S_VERIFY_TOKEN.result)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace ProjectJ

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Message_2eproto