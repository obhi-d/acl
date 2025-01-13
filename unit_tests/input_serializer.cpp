
#include "acl/containers/array_types.hpp"
#include "acl/reflection/reflection.hpp"
#include "acl/serializers/serializers.hpp"
#include "acl/utility/optional_ref.hpp"
#include <catch2/catch_all.hpp>
#include <charconv>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

// NOLINTBEGIN
struct InputData
{
  json root;
};

class Serializer
{
public:
  Serializer() noexcept = default;
  Serializer(InputData& r) : owner(r), value(std::cref(r.root)) {}
  Serializer(InputData& r, json const& source) : owner(r), value(std::cref(source)) {}

  bool is_object() const noexcept
  {
    return value.get().is_object();
  }

  bool is_array() const noexcept
  {
    return value.get().is_array();
  }

  bool is_null() const noexcept
  {
    return value.get().is_null();
  }

  uint32_t size() const noexcept
  {
    return static_cast<uint32_t>(value.get().size());
  }

  template <typename L>
    requires(acl::function_traits<L>::arity == 2)
  void for_each(L&& lambda) const noexcept
  {
    assert(value.get().is_object());
    for (auto const& [key, value] : value.get().items())
    {
      lambda(std::string_view(key), Serializer(owner, value));
    }
  }

  template <typename L>
    requires(acl::function_traits<L>::arity == 1)
  void for_each(L&& lambda) const noexcept
  {
    for (auto const& value : value.get())
    {
      lambda(Serializer(owner, value));
    }
  }

  std::optional<Serializer> at(std::string_view name) const noexcept
  {
    auto it = value.get().find(name);
    if (it != value.get().end())
      return Serializer(owner, *it);
    return {};
  }

  std::optional<Serializer> at(uint32_t idx) const noexcept
  {
    if (idx >= value.get().size())
      return {};
    return Serializer(owner, value.get().at(idx));
  }

  auto as_double() const noexcept
  {
    return acl::optional_ref(value.get().get_ptr<json::number_float_t const*>());
  }

  auto as_uint64() const noexcept
  {
    return acl::optional_ref(value.get().get_ptr<json::number_unsigned_t const*>());
  }

  auto as_int64() const noexcept
  {
    return acl::optional_ref(value.get().get_ptr<json::number_integer_t const*>());
  }

  auto as_bool() const noexcept
  {
    return acl::optional_ref(value.get().get_ptr<json::boolean_t const*>());
  }

  auto as_string() const noexcept
  {
    return acl::optional_ref(value.get().get_ptr<json::string_t const*>());
  }

private:
  std::reference_wrapper<InputData>  owner;
  std::reference_wrapper<json const> value;
};

enum class EnumTest
{
  value0 = 323,
  value1 = 43535,
  value3 = 64533
};

struct ReflTestFriend
{
  int      a  = 0;
  int      b  = 0;
  EnumTest et = EnumTest::value0;
};

template <>
constexpr auto acl::reflect<ReflTestFriend>() noexcept
{
  return acl::bind(acl::bind<"a", &ReflTestFriend::a>(), acl::bind<"b", &ReflTestFriend::b>(),
                   acl::bind<"et", &ReflTestFriend::et>());
}

TEST_CASE("structured_input_serializer: Test valid stream in with reflect outside")
{
  json j = "{ \"a\": 100, \"b\": 200, \"et\": 64533 }"_json;

  InputData input;
  input.root                = j;
  auto           serializer = Serializer(input);
  ReflTestFriend myStruct;
  acl::read(serializer, myStruct);

  REQUIRE(myStruct.a == 100);
  REQUIRE(myStruct.b == 200);
  REQUIRE(myStruct.et == EnumTest::value3);
}

TEST_CASE("structured_input_serializer: Test partial stream in with reflect outside")
{
  json j = "{ \"a\": 100 }"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestFriend myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.a == 100);
  REQUIRE(myStruct.b == 0);
}

TEST_CASE("structured_input_serializer: Test fail stream in with reflect outside")
{
  json j = "{ \"a\": \"is_string\" }"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestFriend myStruct;

  REQUIRE_THROWS(acl::read(serializer, myStruct));
}

class ReflTestClass
{
  int a = 0;
  int b = 1;

public:
  int get_a() const
  {
    return a;
  }
  int get_b() const
  {
    return b;
  }
  static auto reflect() noexcept
  {
    return acl::bind(acl::bind<"a", &ReflTestClass::a>(), acl::bind<"b", &ReflTestClass::b>());
  }
};

TEST_CASE("structured_input_serializer: Test valid stream in with reflect member")
{
  json j = "{ \"a\": 100, \"b\": 200 }"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestClass myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.get_a() == 100);
  REQUIRE(myStruct.get_b() == 200);
}

struct ReflTestMember
{
  ReflTestClass first;
  ReflTestClass second;

  static auto reflect() noexcept
  {
    return acl::bind(acl::bind<"first", &ReflTestMember::first>(), acl::bind<"second", &ReflTestMember::second>());
  }
};

TEST_CASE("structured_input_serializer: Test 1 level scoped class")
{
  json j = R"({ "first":{ "a": 100, "b": 200 }, "second":{ "a": 300, "b": 400 } } )"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestMember myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.first.get_a() == 100);
  REQUIRE(myStruct.first.get_b() == 200);
  REQUIRE(myStruct.second.get_a() == 300);
  REQUIRE(myStruct.second.get_b() == 400);
}

TEST_CASE("structured_input_serializer: Test partial 1 level scoped class")
{
  json j = R"({ "first":{ "a": 100, "b": 200 } } )"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestMember myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.first.get_a() == 100);
  REQUIRE(myStruct.first.get_b() == 200);
  REQUIRE(myStruct.second.get_a() == 0);
  REQUIRE(myStruct.second.get_b() == 1);
}

struct ReflTestClass2
{
  ReflTestMember first;
  std::string    second;

  static auto reflect() noexcept
  {
    return acl::bind(acl::bind<"first", &ReflTestClass2::first>(), acl::bind<"second", &ReflTestClass2::second>());
  }
};

TEST_CASE("structured_input_serializer: Test 2 level scoped class")
{
  json j = R"({ "first":{ "first":{ "a": 100, "b": 200 }, "second":{ "a": 300, "b": 400 } }, "second":"value" })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  ReflTestClass2 myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.first.first.get_a() == 100);
  REQUIRE(myStruct.first.first.get_b() == 200);
  REQUIRE(myStruct.first.second.get_a() == 300);
  REQUIRE(myStruct.first.second.get_b() == 400);
  REQUIRE(myStruct.second == "value");
}

TEST_CASE("structured_input_serializer: Test pair")
{
  json j = R"([ { "first":{ "a": 100, "b": 200 }, "second":{ "a": 300, "b": 400 } }, "value" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  std::pair<ReflTestMember, std::string> myStruct;

  acl::read(serializer, myStruct);

  REQUIRE(myStruct.first.first.get_a() == 100);
  REQUIRE(myStruct.first.first.get_b() == 200);
  REQUIRE(myStruct.first.second.get_a() == 300);
  REQUIRE(myStruct.first.second.get_b() == 400);
  REQUIRE(myStruct.second == "value");
}

TEST_CASE("structured_input_serializer: TupleLike ")
{
  json j = R"([ { "first":{ "a": 100, "b": 200 }, "second":{ "a": 300, "b": 400 } }, "value", 324, true ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  std::tuple<ReflTestMember, std::string, int, bool> myStruct = {};

  acl::read(serializer, myStruct);

  REQUIRE(std::get<0>(myStruct).first.get_a() == 100);
  REQUIRE(std::get<0>(myStruct).first.get_b() == 200);
  REQUIRE(std::get<0>(myStruct).second.get_a() == 300);
  REQUIRE(std::get<0>(myStruct).second.get_b() == 400);
  REQUIRE(std::get<1>(myStruct) == "value");
  REQUIRE(std::get<2>(myStruct) == 324);
  REQUIRE(std::get<3>(myStruct) == true);
}

TEST_CASE("structured_input_serializer: Invalid TupleLike ")
{
  json j = R"({ "first": "invalid" })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  std::tuple<ReflTestMember, std::string, int, bool> myStruct = {};

  REQUIRE_THROWS(acl::read(serializer, myStruct));
}

TEST_CASE("structured_input_serializer: StringMapLike ")
{
  using pair_type = std::pair<int, std::string>;
  using type      = std::unordered_map<std::string, pair_type>;
  json j          = R"({ "first":[ 100, "100"], "second":[ 300, "300" ] , "third":[ 400, "400" ] })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  type myMap = {};

  acl::read(serializer, myMap);

  REQUIRE(myMap["first"] == pair_type(100, "100"));
  REQUIRE(myMap["second"] == pair_type(300, "300"));
  REQUIRE(myMap["third"] == pair_type(400, "400"));
}

TEST_CASE("structured_input_serializer: StringMapLike Invalid ")
{
  using pair_type = std::pair<int, std::string>;
  using type      = std::unordered_map<std::string, pair_type>;
  json j          = R"({ "first":"invalid", "second":[ 300, "300" ] , "third":[ 400, "400" ] })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  type myMap = {};

  REQUIRE_THROWS(acl::read(serializer, myMap));
}

TEST_CASE("structured_input_serializer: StringMapLike Invalid  Subelement")
{
  using pair_type = std::pair<int, std::string>;
  using type      = std::unordered_map<std::string, pair_type>;
  json j          = R"([ "invalid", [ 300, "300" ] , [ 400, "400" ] ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  type myMap = {};

  REQUIRE_THROWS(acl::read(serializer, myMap));
}

TEST_CASE("structured_input_serializer: ArrayLike")
{
  using pair_type = std::pair<int, std::string>;
  using type      = std::unordered_map<int, pair_type>;
  json j =
   R"([ {"key":11, "value":[ 100, "100"]}, {"key":13, "value":[ 300, "300" ]} , {"key":15, "value":[ 400, "400" ]} ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  type myMap = {};

  acl::read(serializer, myMap);

  REQUIRE(myMap[11] == pair_type(100, "100"));
  REQUIRE(myMap[13] == pair_type(300, "300"));
  REQUIRE(myMap[15] == pair_type(400, "400"));
}

TEST_CASE("structured_input_serializer: ArrayLike (no emplace)")
{
  acl::dynamic_array<int> myArray;
  json                    j = R"([ 11, 100, 13, 300 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray.size() == 4);
  REQUIRE(myArray[0] == 11);
  REQUIRE(myArray[1] == 100);
  REQUIRE(myArray[2] == 13);
  REQUIRE(myArray[3] == 300);
}

TEST_CASE("structured_input_serializer: ArrayLike Invalid ")
{
  acl::dynamic_array<int> myArray;
  json                    j = R"({ })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));

  REQUIRE(myArray.empty());
}

TEST_CASE("structured_input_serializer: ArrayLike (no emplace) Invalid Subelement ")
{
  acl::dynamic_array<int> myArray;
  json                    j = R"([ "string", 100, 13, 300 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));

  REQUIRE(myArray.empty());
}

TEST_CASE("structured_input_serializer: ArrayLike Invalid Subelement ")
{
  using pair_type = std::pair<int, std::string>;
  using type      = std::unordered_map<int, pair_type>;
  json j          = R"([ [11, [ 100, 100]], [13, [ 300, "300" ]] , [15, [ 400, "400" ]] ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  type myMap = {};

  REQUIRE_THROWS(acl::read(serializer, myMap));
}

TEST_CASE("structured_input_serializer: VariantLike ")
{
  std::vector<std::variant<int, bool, std::string>> variantList;

  json j =
   R"([ {"type":0, "value":100 }, {"type":1, "value":true}, {"type":2, "value":"100" }, { "type":1, "value":false } ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, variantList);

  REQUIRE(variantList.size() == 4);
  REQUIRE(std::holds_alternative<int>(variantList[0]));
  REQUIRE(std::holds_alternative<bool>(variantList[1]));
  REQUIRE(std::holds_alternative<std::string>(variantList[2]));
  REQUIRE(std::holds_alternative<bool>(variantList[3]));
  REQUIRE(std::get<int>(variantList[0]) == 100);
  REQUIRE(std::get<bool>(variantList[1]) == true);
  REQUIRE(std::get<std::string>(variantList[2]) == "100");
  REQUIRE(std::get<bool>(variantList[3]) == false);
}

TEST_CASE("structured_input_serializer: VariantLike Invalid")
{
  std::variant<int, bool, std::string> variant;

  json j = R"({ "type":"value", "value":"100" })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, variant));
}

TEST_CASE("structured_input_serializer: VariantLike Missing Type")
{
  std::variant<int, bool, std::string> variant;

  json j = R"({ "value": "100" })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, variant));
}

TEST_CASE("structured_input_serializer: VariantLike Missing Value")
{
  std::variant<int, bool, std::string> variant;

  json j = R"({ "type": 1 })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, variant));
}

TEST_CASE("structured_input_serializer: VariantLike Invalid Type")
{
  std::variant<int, bool, std::string> variant;

  json j = R"([ 0, "value", "100" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, variant));
}

struct ConstructedSV
{
  int id                   = -1;
  ConstructedSV() noexcept = default;
  explicit ConstructedSV(std::string_view sv)
  {
    std::from_chars(sv.data(), sv.data() + sv.length(), id);
  }
};

TEST_CASE("structured_input_serializer: ConstructedFromStringView")
{
  acl::dynamic_array<ConstructedSV> myArray;
  json                              j = R"([ "11", "100", "13", "300" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray.size() == 4);
  REQUIRE(myArray[0].id == 11);
  REQUIRE(myArray[1].id == 100);
  REQUIRE(myArray[2].id == 13);
  REQUIRE(myArray[3].id == 300);
}

TEST_CASE("structured_input_serializer: ConstructedFromStringView Invalid")
{
  acl::dynamic_array<ConstructedSV> myArray;
  json                              j = R"([ 11, "100", "13", "300" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));

  REQUIRE(myArray.empty());
}

struct TransformSV
{
  int id                 = -1;
  TransformSV() noexcept = default;
};

template <>
struct acl::convert<TransformSV>
{
  static std::string to_string(TransformSV const& r)
  {
    return std::to_string(r.id);
  }

  static void from_string(TransformSV& r, std::string_view sv)
  {
    std::from_chars(sv.data(), sv.data() + sv.length(), r.id);
  }
};

TEST_CASE("structured_input_serializer: TransformFromString")
{
  acl::dynamic_array<TransformSV> myArray;
  json                            j = R"([ "11", "100", "13", "300" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray.size() == 4);
  REQUIRE(myArray[0].id == 11);
  REQUIRE(myArray[1].id == 100);
  REQUIRE(myArray[2].id == 13);
  REQUIRE(myArray[3].id == 300);
}

TEST_CASE("structured_input_serializer: TransformFromString Invalid")
{
  acl::dynamic_array<TransformSV> myArray;
  json                            j = R"([ 11, "100", "13", "300" ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));
}

TEST_CASE("structured_input_serializer: BoolLike")
{
  std::array<bool, 4> myArray = {};
  json                j       = R"([ false, true, false, true ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray.size() == 4);
  REQUIRE(myArray[0] == false);
  REQUIRE(myArray[1] == true);
  REQUIRE(myArray[2] == false);
  REQUIRE(myArray[3] == true);
}

TEST_CASE("structured_input_serializer: BoolLike Invaild")
{
  std::array<bool, 4> myArray = {};
  json                j       = R"([ 1, true, false, true ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));
}

TEST_CASE("structured_input_serializer: SignedIntLike")
{
  std::array<int, 4> myArray = {};
  json               j       = R"([ -40, -10, 10, 40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray[0] == -40);
  REQUIRE(myArray[1] == -10);
  REQUIRE(myArray[2] == 10);
  REQUIRE(myArray[3] == 40);
}

TEST_CASE("structured_input_serializer: SignedIntLike Invalid")
{
  std::array<int, 4> myArray = {};
  json               j       = R"([ "-40", -10, 10, 40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));
}

TEST_CASE("structured_input_serializer: UnsignedIntLike")
{
  std::array<uint32_t, 4> myArray = {};
  json                    j       = R"([ 40, 10, 10, 40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray[0] == 40);
  REQUIRE(myArray[1] == 10);
  REQUIRE(myArray[2] == 10);
  REQUIRE(myArray[3] == 40);
}

TEST_CASE("structured_input_serializer: UnsignedIntLike Invalid")
{
  std::array<uint32_t, 4> myArray = {};
  json                    j       = R"([ true, 10, 10, 40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));
}

TEST_CASE("structured_input_serializer: FloatLike")
{
  std::array<float, 4> myArray = {};
  json                 j       = R"([ 434.442, 757.10, 10.745, 424.40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, myArray);

  REQUIRE(myArray[0] == Catch::Approx(434.442f));
  REQUIRE(myArray[1] == Catch::Approx(757.10f));
  REQUIRE(myArray[2] == Catch::Approx(10.745f));
  REQUIRE(myArray[3] == Catch::Approx(424.40f));
}

TEST_CASE("structured_input_serializer: FloatLike Invalid")
{
  std::array<float, 4> myArray = {};
  json                 j       = R"([ 434, 757.10, 10.745, 424.40 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  REQUIRE_THROWS(acl::read(serializer, myArray));
}

TEST_CASE("structured_input_serializer: PointerLike")
{
  struct pointer
  {
    std::shared_ptr<std::string> a;
    std::unique_ptr<std::string> b;
  };

  pointer pvalue;
  json    j = R"({ "a":"A_value", "b":"B_value", "c":"C_value" })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, pvalue);

  REQUIRE(pvalue.a);
  REQUIRE(pvalue.b);
  REQUIRE(*pvalue.a == "A_value");
  REQUIRE(*pvalue.b == "B_value");
}

TEST_CASE("structured_input_serializer: PointerLike Null")
{
  struct pointer
  {
    std::shared_ptr<std::string> a;
    std::unique_ptr<std::string> b;

    static auto reflect() noexcept
    {
      return acl::bind(acl::bind<"a", &pointer::a>(), acl::bind<"b", &pointer::b>());
    }
  };

  pointer pvalue;
  json    j = R"({ "a":null, "b":null, "c":null })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, pvalue);

  REQUIRE(!pvalue.a);
  REQUIRE(!pvalue.b);
}

TEST_CASE("structured_input_serializer: OptionalLike")
{
  struct pointer
  {
    std::optional<std::string> a;
    std::optional<std::string> b;

    static auto reflect() noexcept
    {
      return acl::bind(acl::bind<"a", &pointer::a>(), acl::bind<"b", &pointer::b>());
    }
  };

  pointer pvalue;
  json    j = R"({ "a":"A_value", "b":null })"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, pvalue);

  REQUIRE(pvalue.a);
  REQUIRE(!pvalue.b);
  REQUIRE(*pvalue.a == "A_value");
}

class CustomClass
{
public:
  friend Serializer& operator>>(Serializer& ser, CustomClass& cc)
  {
    cc.value = (int)(*ser.as_int64());
    return ser;
  }

  inline int get() const
  {
    return value;
  }

private:
  int value;
};

TEST_CASE("structured_input_serializer: InputSerializableClass")
{
  std::vector<CustomClass> integers;
  json                     j = R"([ 34, 542, 234 ])"_json;

  InputData input;
  input.root      = j;
  auto serializer = Serializer(input);

  acl::read(serializer, integers);

  REQUIRE(integers.size() == 3);
  REQUIRE(integers[0].get() == 34);
  REQUIRE(integers[1].get() == 542);
  REQUIRE(integers[2].get() == 234);
}
// NOLINTEND