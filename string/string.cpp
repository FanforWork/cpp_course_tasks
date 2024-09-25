#include "string.hpp"
String::String() : size_{0}, capacity_{0}, buffer_{nullptr} {}

String::String(size_t size, char character) {
  size_ = size;
  capacity_ = size;
  buffer_ = new char[capacity_ + 1];
  for (size_t i = 0; i < capacity_; ++i) {
    buffer_[i] = character;
  }
  buffer_[capacity_] = '\0';
}

String::String(const char* str) : size_(strlen(str)), capacity_(size_) {
  if (size_ != 0) {
    buffer_ = new char[size_ + 1];
    for (size_t i = 0; i < size_; ++i) {
      buffer_[i] = str[i];
    }
    NullTerminator();
  } else {
    buffer_ = nullptr;
  }
}

void String::Clear() { size_ = 0; }

void String::NullTerminator() { buffer_[size_] = '\0'; }

void String::Resize(size_t new_size) {
  Reserve(new_size);
  size_ = new_size;
  buffer_[new_size] = '\0';
}

void String::PushBack(char character) {
  if (++size_ >= capacity_) {
    if (capacity_ == 0) {
      Reserve(1);
    } else {
      Reserve(2 * capacity_);
    }
  }
  buffer_[size_ - 1] = character;
  NullTerminator();
}

void String::PopBack() {
  if (!(Empty())) {
    --size_;
    NullTerminator();
  }
}

void String::Resize(size_t new_size, char character) {
  if (new_size > capacity_) {
    char* tmp = buffer_;
    buffer_ = new char[new_size + 1];
    capacity_ = new_size;
    memcpy(buffer_, tmp, size_);
    for (size_t i = size_; i < new_size; ++i) {
      buffer_[i] = character;
    }
    delete[] tmp;
    size_ = new_size;
  }
  Resize(new_size);
}
char& String::Front() { return buffer_[0]; }

const char& String::Front() const { return buffer_[0]; }

char& String::Back() { return buffer_[size_ - 1]; }

const char& String::Back() const { return buffer_[size_ - 1]; }

bool String::Empty() const { return size_ == 0; }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

const char* String::Data() const { return buffer_; }

char* String::Data() { return buffer_; }

String::String(const String& other_string)
    : size_(other_string.size_), capacity_(other_string.capacity_) {
  if (other_string.buffer_ != nullptr) {
    buffer_ = new char[capacity_ + 1];
    memcpy(buffer_, other_string.buffer_, size_);
    NullTerminator();
  } else {
    buffer_ = nullptr;
  }
}

String& String::operator=(const String& str) {
  // if(this != str ) {
  size_ = str.size_;
  capacity_ = str.capacity_;
  if (str.buffer_ != nullptr) {
    delete[] buffer_;
    buffer_ = new char[capacity_ + 1];
    memcpy(buffer_, str.buffer_, size_ + 1);
    NullTerminator();
  } else {
    buffer_ = nullptr;
  }
  return *this;
  // }
  // return *this;
}
String::~String() { delete[] buffer_; }

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    capacity_ = new_cap;
    char* buffer_new = buffer_;
    buffer_ = new char[new_cap + 1];
    if (buffer_new != nullptr) {
      memcpy(buffer_, buffer_new, size_);
    }
    delete[] buffer_new;
  }
  if (capacity_ != 0) {
    NullTerminator();
  }
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    capacity_ = size_;
    char* buffer_new = buffer_;
    buffer_ = new char[size_ + 1];
    if (buffer_new != nullptr) {
      memcpy(buffer_, buffer_new, size_);
    }
    delete[] buffer_new;
  }
  if (buffer_ != nullptr) {
    NullTerminator();
  }
}

void String::Swap(String& other_string) {
  std::swap(other_string.buffer_, buffer_);
  std::swap(other_string.size_, size_);
  std::swap(other_string.capacity_, capacity_);
}
const char& String::operator[](size_t index) const { return buffer_[index]; }
char& String::operator[](size_t index) { return buffer_[index]; }
bool String::operator>(const String& other_string) const {
  if (buffer_ == nullptr && other_string.buffer_ != nullptr) {
    return false;
  }
  if (buffer_ != nullptr && other_string.buffer_ == nullptr) {
    return true;
  }
  if (buffer_ == nullptr && other_string.buffer_ == nullptr) {
    return false;
  }
  return (strcmp(buffer_, other_string.buffer_) > 0);
}
bool String::operator<(const String& other_string) const {
  return (other_string > *this);
}
bool String::operator==(const String& other_string) const {
  return *this <= other_string && other_string <= *this;
}
bool String::operator<=(const String& other_string) const {
  return !(*this > other_string);
}
bool String::operator>=(const String& other_string) const {
  return !(*this < other_string);
}
bool String::operator!=(const String& other_string) const {
  return !(*this == other_string);
}
String& String::operator+=(const String& string) {
  for (size_t i = 0; i < string.size_; ++i) {
    this->PushBack(string.buffer_[i]);
  }
  return *this;
}
String String::operator+(const String& string) const {
  String copy = *this;
  return copy += string;
}

String& String::operator*=(size_t n) {
  String res;
  if (n == 0) {
    Clear();
    return *this;
  }
  for (size_t i = 0; i < n; ++i) {
    res += *this;
  }
  *this = res;
  return *this;
}
String String::operator*(size_t n) const {
  String copy = *this;
  copy *= n;
  return copy;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> partical;
  size_t counter = 0;
  for (size_t j = 0; j < size_ - (delim.size_ - 1); ++j) {
    if (memcmp(buffer_ + j, delim.buffer_, delim.size_) == 0) {
      String output;
      output.Reserve(j - counter);
      output.size_ = j - counter;
      memcpy(output.buffer_, buffer_ + counter, j - counter);
      counter = j + delim.size_;
      partical.push_back(output);
    }
  }
  String output;
  output.Reserve(size_ - counter);
  output.size_ = size_ - counter;
  memcpy(output.buffer_, buffer_ + counter, size_ - counter);
  partical.push_back(output);
  return partical;
}

String String::Join(const std::vector<String>& strings) const {
  if (strings.empty()) {
    return "";
  }
  if (strings.size() == 1) {
    return strings[0];
  }
  String res;
  for (size_t i = 0; i < strings.size() - 1; ++i) {
    res += strings[i] + *this;
  }
  res += strings[strings.size() - 1];
  return res;
}

std::ostream& operator<<(std::ostream& out, const String& string) {
  out << string.buffer_;
  return out;
}
std::istream& operator>>(std::istream& input, String& string) {
  string.Clear();
  char character;
  while ((input.get(character)) && !(input.eof())) {
    string.PushBack(character);
  }
  return input;
}
