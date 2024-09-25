#pragma once
#include <cstring>
#include <iostream>
#include <vector>
class String {
 public:
  String();
  String(size_t size, char character);
  String(const char* str);
  String(const String& other_string);
  ~String();
  void Resize(size_t new_size, char character);
  void PushBack(char character);
  void PopBack();
  void Clear();
  void Resize(size_t new_size);
  void ShrinkToFit();
  void Reserve(size_t new_cap);
  void Swap(String& other_string);
  bool operator<(const String& other_string) const;
  bool operator>(const String& other_string) const;
  bool operator<=(const String& other_string) const;
  bool operator>=(const String& other_string) const;
  bool operator==(const String& other_string) const;
  bool operator!=(const String& other_string) const;
  bool Empty() const;
  char& operator[](size_t ind);
  const char& operator[](size_t ind) const;
  char& Front();
  const char& Front() const;
  char& Back();
  const char& Back() const;
  const char* Data() const;
  char* Data();
  size_t Size() const;
  size_t Capacity() const;
  String operator+(const String& string) const;
  String& operator+=(const String& string);
  String operator*(size_t n) const;
  String& operator*=(size_t n);
  String& operator=(const String& str);
  friend std::ostream& operator<<(std::ostream& out, const String& string);
  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings) const;

 private:
  size_t size_;
  size_t capacity_;
  char* buffer_;
  void NullTerminator();
};
std::istream& operator>>(std::istream& input, String& string);