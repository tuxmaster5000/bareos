/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2018-2024 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
#ifndef BAREOS_LIB_UTIL_H_
#define BAREOS_LIB_UTIL_H_

#include <sys/stat.h>
#include <chrono>
#include <variant>
#include <chrono>
#include <optional>

#if defined(HAVE_WIN32)
#  include "bregex.h"
#else
#  include <regex.h>
#endif

#include "lib/ascii_control_characters.h"
#include "lib/message.h"

class BareosSocket;
class ConfigurationParser;
class QualifiedResourceNameTypeConverter;
enum class BareosVersionNumber : uint32_t;

void EscapeString(PoolMem& snew, const char* old, int len);
std::string EscapeString(const char* old);
bool IsBufZero(char* buf, int len);
void lcase(char* str);
void BashSpaces(char* str);
void BashSpaces(std::string& str);
void BashSpaces(PoolMem& pm);
void UnbashSpaces(char* str);
void UnbashSpaces(PoolMem& pm);
bool GetNameAndResourceTypeAndVersionFromHello(const std::string& input,
                                               std::string& name,
                                               std::string& r_type_str,
                                               BareosVersionNumber& version);
const char* IndentMultilineString(PoolMem& resultbuffer,
                                  const char* multilinestring,
                                  const char* separator);
char* encode_time(utime_t time, char* buf);
bool ConvertTimeoutToTimespec(timespec& timeout, int timeout_in_seconds);
char* encode_mode(mode_t mode, char* buf);
int DoShellExpansion(char* name, int name_len);
std::string JobstatusToAscii(int JobStatus);
int RunProgram(char* prog, int wait, POOLMEM*& results);
int RunProgramFullOutput(char* prog, int wait, POOLMEM*& results);
char* action_on_purge_to_string(int aop, PoolMem& ret);
const char* job_type_to_str(int type);
const char* job_replace_to_str(int relace);
const char* job_status_to_str(int stat);
const char* job_level_to_str(int level);
bool MakeSessionKey(char key[120]);
POOLMEM* edit_job_codes(JobControlRecord* jcr,
                        char* omsg,
                        const char* imsg,
                        const char* to,
                        job_code_callback_t job_code_callback = NULL);
void SetWorkingDirectory(const char* wd);
const char* last_path_separator(const char* str);
void SortCaseInsensitive(std::vector<std::string>& v);
std::string getenv_std_string(std::string env_var);
void StringToLowerCase(std::string& s);
void StringToLowerCase(std::string& out, const std::string& in);
bool pm_append(void* pm_string, const char* fmt, ...);
std::vector<std::string> split_string(const std::string& str, char delim);

std::string CreateDelimitedStringForSqlQueries(
    const std::vector<char>& elements,
    char delim);

std::string TPAsString(const std::chrono::system_clock::time_point& tp);
regex_t* StringToRegex(const char* input);
void to_lower(std::string& s);

// see N3876 / boost::hash_combine
inline std::size_t hash_combine(std::size_t seed, std::size_t hash)
{
  // 0x9e3779b9 is approximately 2^32 / phi (where phi is the golden ratio)
  std::size_t changed = hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed ^ changed;
}

// fixme(ssura): replace by std::expected<T, PoolMem> (C++23)
//               or std::expected<T, std::string>
template <typename T> class result {
 public:
  constexpr result() : data{std::in_place_index<1>, "Not Initialized"} {}

  template <typename Arg0, typename... Args>
  constexpr result(Arg0 arg0, Args... args)
      : data(std::forward<Arg0>(arg0), std::forward<Args>(args)...)
  {
  }

  bool holds_error() const { return data.index() == 1; }
  PoolMem* error()
  {
    if (holds_error()) {
      return &std::get<1>(data);
    } else {
      return nullptr;
    }
  }

  T* value()
  {
    if (!holds_error()) {
      return &std::get<0>(data);
    } else {
      return nullptr;
    }
  }

  PoolMem& error_unchecked() { return std::get<1>(data); }

  T& value_unchecked() { return std::get<0>(data); }

 private:
  // we are using PoolMem here, since it is easier to format into a PoolMem.
  // Once we have access to std::format everywhere, this should change
  // to std::string.
  std::variant<T, PoolMem> data;
};

class timer {
 public:
  timer() { reset_and_start(); }
  void reset_and_start();
  void stop();

  const char* format_human_readable();

 private:
  using time_point = std::chrono::steady_clock::time_point;
  time_point start{};
  std::optional<time_point> end{};
  std::string formatted{};
};

#endif  // BAREOS_LIB_UTIL_H_
