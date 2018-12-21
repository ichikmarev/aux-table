#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <iomanip>
#include "char_conv.h"
#include "map_as_vector.h"
#include "segment.h"
#include "group_pairs.h"
#include "create_permutation_tree.h"
#include "permutation_tree_to_permutation.h"
#include "permutation.h"
#include "create_permutation.h"
#include "myconcepts.h"
#include "list_to_columns.h"
#include "format.h"


enum Category : uint16_t{
    Spaces,             Other,           Delimiters,
    Id_begin,           Id_body,         Hex_digits,
    Dec_digits,         Oct_digits,      Bin_digits,
    Comma
};

static const char32_t* id_begin_chars =
    U"ЀЁЂЃЄЅІЇЈЉЊЋЌЍЎЏАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяѐёђѓєѕіїјљњћќѝўџѠѡѢѣѤѥѦѧѨѩѪѫѬѭѮѯѰѱѲѳѴѵѶѷѸѹѺѻѼѽѾѿҀҁ҂҃҄҅҆҇҈҉ҊҋҌҍҎҏҐґҒғҔҕҖҗҘҙҚқҜҝҞҟҠҡҢңҤҥҦҧҨҩҪҫҬҭҮүҰұҲҳҴҵҶҷҸҹҺһҼҽҾҿӀӁӂӃӄӅӆӇӈӉӊӋӌӍӎӏӐӑӒӓӔӕӖӗӘәӚӛӜӝӞӟӠӡӢӣӤӥӦӧӨөӪӫӬӭӮӯӰӱӲӳӴӵӶӷӸӹӺӻӼӽӾӿ
"
    U"_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    U"ƀƁƂƃƄƅƆƇƈƉƊƋƌƍƎƏƐƑƒƓƔƕƖƗƘƙƚƛƜƝƞƟƠơƢƣƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽƾƿǀǁǂǃǄǅǆǇǈǉǊǋǌǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǲǳǴǵǶǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄɅɆɇɈɉɊɋɌɍɎɏ"
    U"ͰͱͲͳʹ͵Ͷͷͺͻͼͽ;Ϳ΄΅Ά·ΈΉΊ Ό ΎΏΐΑΒΓΔΕΖΗΘΙΚΛΜΝ0ΠΡΣΤΥΦΧΨΩΪΫάέήίΰαβγδεζηθικλμνξοπρςστυφχψωϊϋόύώϏϐϑϒϓϔϕϖϗϘϙϚϛϜϝϞϟϠϡϢϣϤϥϦϧϨϩϪϫϬϭϮϯϰϱϲϳϴϵ϶ϷϸϹϺϻϼϽϾϿ"
    U"_"

static const char32_t* id_body_chars =
    U"ЀЁЂЃЄЅІЇЈЉЊЋЌЍЎЏАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяѐёђѓєѕіїјљњћќѝўџѠѡѢѣѤѥѦѧѨѩѪѫѬѭѮѯѰѱѲѳѴѵѶѷѸѹѺѻѼѽѾѿҀҁ҂҃҄҅҆҇҈҉ҊҋҌҍҎҏҐґҒғҔҕҖҗҘҙҚқҜҝҞҟҠҡҢңҤҥҦҧҨҩҪҫҬҭҮүҰұҲҳҴҵҶҷҸҹҺһҼҽҾҿӀӁӂӃӄӅӆӇӈӉӊӋӌӍӎӏӐӑӒӓӔӕӖӗӘәӚӛӜӝӞӟӠӡӢӣӤӥӦӧӨөӪӫӬӭӮӯӰӱӲӳӴӵӶӷӸӹӺӻӼӽӾӿ
    "
    U"_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    U"ƀƁƂƃƄƅƆƇƈƉƊƋƌƍƎƏƐƑƒƓƔƕƖƗƘƙƚƛƜƝƞƟƠơƢƣƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽƾƿǀǁǂǃǄǅǆǇǈǉǊǋǌǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǲǳǴǵǶǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄɅɆɇɈɉɊɋɌɍɎɏ"
    U"ͰͱͲͳʹ͵Ͷͷͺͻͼͽ;Ϳ΄΅Ά·ΈΉΊ Ό ΎΏΐΑΒΓΔΕΖΗΘΙΚΛΜΝ0ΠΡΣΤΥΦΧΨΩΪΫάέήίΰαβγδεζηθικλμνξοπρςστυφχψωϊϋόύώϏϐϑϒϓϔϕϖϗϘϙϚϛϜϝϞϟϠϡϢϣϤϥϦϧϨϩϪϫϬϭϮϯϰϱϲϳϴϵ϶ϷϸϹϺϻϼϽϾϿ"
    U"_"
    U"0123456789"
    U"0123456789ABCDEF"
    U"01234567"
    U"01"

static const char32_t* hex_digits = U"0123456789ABCDEF";
static const char32_t* dec_digits = U"0123456789";
static const char32_t* oct_digits = U"01234567";
static const char32_t* bin_digits = U"01";

static const char32_t* delimiters_chars = U"{}()|*+?><\\*^$%,;:";

static const char32_t* cyrilic_chars = U"ƀƁƂƃƄƅƆƇƈƉƊƋƌƍƎƏƐƑƒƓƔƕƖƗƘƙƚƛƜƝƞƟƠơƢƣƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽƾƿǀǁǂǃǄǅǆǇǈǉǊǋǌǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǲǳǴǵǶǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄɅɆɇɈɉɊɋɌɍɎɏ"
static const char32_t* greek_chars = U"ͰͱͲͳʹ͵Ͷͷͺͻͼͽ;Ϳ΄΅Ά·ΈΉΊ Ό ΎΏΐΑΒΓΔΕΖΗΘΙΚΛΜΝ0ΠΡΣΤΥΦΧΨΩΪΫάέήίΰαβγδεζηθικλμνξοπρςστυφχψωϊϋόύώϏϐϑϒϓϔϕϖϗϘϙϚϛϜϝϞϟϠϡϢϣϤϥϦϧϨϩϪϫϬϭϮϯϰϱϲϳϴϵ϶ϷϸϹϺϻϼϽϾϿ"
static const char32_t* latin_chars = U"_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

//мнемоники

std::map<char32_t, uint16_t> table;

static void insert_char(const char32_t ch, Category category){
    auto it = table.find(ch);
    if(it != table.end()){
        table[ch] |= 1U << category;
    }else{
        table[ch] =  1U << category;
    }
}

static void add_category(const char32_t* p, Category category){
    while(char32_t ch = *p++){
        insert_char(ch, category);
    }
}

static std::u32string spaces_str(){
    std::u32string s;
    for(char32_t c = 1; c <= U' '; c++){
        s += c;
    }
    return s;
}

static void fill_table(){
    std::u32string s = spaces_str();

    add_category(s.c_str(),             Spaces);
    add_category(id_begin_chars,        Id_begin);
    add_category(id_body_chars,         Id_body);
    add_category(delimiters_chars,      Delimiters);
    add_category(after_colon_chars,     After_colon);
    add_category(after_backslash_chars, After_backslash);
    add_category(U"[",                  Opened_square_br);
    add_category(U"]",                  Closed_square_br);
    add_category(U"\'",                 Comma);
    add_category(hex_digits,            Hex_digits);
    add_category(oct_digits,            Oct_digits);
    add_category(bin_digits,            Bin_digits);
    add_category(dec_digits,            Dec_digits);
    add_category(greek_chars,           Greek_chars);
    add_category(latin_chars,           Latin_chars);
    add_category(cyrilic_chars,         Cyrilic_chars);
}

template<RandomAccessIterator DestIt, RandomAccessIterator SrcIt, Callable F>
void permutate(DestIt dest_begin, SrcIt src_begin, SrcIt src_end, F f){
    size_t num_of_elems = src_end - src_begin;
    for(size_t i = 0; i < num_of_elems; ++i){
        dest_begin[f(i)] = src_begin[i];
    }
}

template<Integral K, typename V>
SegmentsV<K, V> create_classification_table(const std::map<K, V>& m){
   SegmentsV<K,V> grouped_pairs = group_pairs(map_as_vector(m)); // map_as_vector работает нормально, а также и group_pairs
   size_t         n             = grouped_pairs.size();
   auto           result        = SegmentsV<K, V>(n);
   auto           perm          = create_permutation(grouped_pairs.size());
   auto           f             = [perm](size_t i) -> size_t{return perm[i];};
   permutate(result.begin(), grouped_pairs.begin(), grouped_pairs.end(), f);
   return result;
}

static const std::map<char32_t, std::string> esc_char_strings = {
    {U'\'',   R"~(U'\'')~"  }, {U'\"',   R"~(U'\"')~"  }, {U'\?',   R"~(U'\?')~"  },
    {U'\\',   R"~(U'\\')~"  }, {U'\0',   R"~(U'\0')~"  }, {U'\a',   R"~(U'\a')~"  },
    {U'\b',   R"~(U'\b')~"  }, {U'\f',   R"~(U'\f')~"  }, {U'\n',   R"~(U'\n')~"  },
    {U'\r',   R"~(U'\r')~"  }, {U'\t',   R"~(U'\t')~"  }, {U'\v',   R"~(U'\v')~"  },
    {U'\x01', R"~(U'\x01')~"}, {U'\x02', R"~(U'\x02')~"}, {U'\x03', R"~(U'\x03')~"},
    {U'\x04', R"~(U'\x04')~"}, {U'\x05', R"~(U'\x05')~"}, {U'\x06', R"~(U'\x06')~"},
    {U'\x0e', R"~(U'\x0e')~"}, {U'\x0f', R"~(U'\x0f')~"}, {U'\x10', R"~(U'\x10')~"},
    {U'\x11', R"~(U'\x11')~"}, {U'\x12', R"~(U'\x12')~"}, {U'\x13', R"~(U'\x13')~"},
    {U'\x14', R"~(U'\x14')~"}, {U'\x15', R"~(U'\x15')~"}, {U'\x16', R"~(U'\x16')~"},
    {U'\x17', R"~(U'\x17')~"}, {U'\x18', R"~(U'\x18')~"}, {U'\x19', R"~(U'\x19')~"},
    {U'\x1a', R"~(U'\x1a')~"}, {U'\x1b', R"~(U'\x1b')~"}, {U'\x1c', R"~(U'\x1c')~"},
    {U'\x1d', R"~(U'\x1d')~"}, {U'\x1e', R"~(U'\x1e')~"}, {U'\x1f', R"~(U'\x1f')~"}
};

static std::string show_char32(const char32_t c)
{
    std::string result;
    auto it = esc_char_strings.find(c);
    if(it != esc_char_strings.end()){
        result = it->second;
    }else{
        result = "U\'" + char32_to_utf8(c) + '\'';
    }
    return result;
}

static const std::string elem_fmt = R"~({{{{{0:<7}, {1:<7}}},  {2:<5}}})~";

static std::string show_table_elem(const Segment_with_value<char32_t, uint16_t>& e)
{
    std::string result;
    auto&       segment = e.bounds;
    result              = fmt::format(elem_fmt,
                                      show_char32(segment.lower_bound),
                                      show_char32(segment.upper_bound),
                                      e.value);
    return result;
}

static const std::string table_fmt = R"~(enum class Category : uint16_t{{
  Spaces,             Other,           Delimiters,
  Id_begin,           Id_body,         Hex_digits,
  Dec_digits,         Oct_digits,      Bin_digits
}};

/*
 * It happens that in std::map<K,V> the key type is integer, and a lot of keys with the
 * same corresponding values. If such a map must be a generated constant, then this map
 * can be optimized. Namely, iterating through a map using range-based for, we will
 * build a std::vector<std::pair<K, V>>. Then we group pairs std::pair<K, V> in pairs
 * in the form (segment, a value of type V), where 'segment' is a struct consisting of
 * lower bound and upper bound. Next, we permute the grouped pair in the such way that
 * in order to search for in the array of the resulting values we can use the algorithm
 * from the answer to exercise 6.2.24 of the book
 * Knuth D.E. The art of computer programming. Volume 3. Sorting and search. ---
 * 2nd ed. --- Addison-Wesley, 1998.
*/

#define RandomAccessIterator typename
#define Callable             typename
#define Integral             typename
template<typename T>
struct Segment{{
    T lower_bound;
    T upper_bound;

    Segment()               = default;
    Segment(const Segment&) = default;
    ~Segment()              = default;
}};

template<typename T, typename V>
struct Segment_with_value{{
    Segment<T> bounds;
    V          value;

    Segment_with_value()                          = default;
    Segment_with_value(const Segment_with_value&) = default;
    ~Segment_with_value()                         = default;
}};

/* This function uses algorithm from the answer to the exercise 6.2.24 of the monography
 *  Knuth D.E. The art of computer programming. Volume 3. Sorting and search. --- 2nd ed.
 *  --- Addison-Wesley, 1998.
*/
template<RandomAccessIterator I, typename K>
std::pair<bool, size_t> knuth_find(I it_begin, I it_end, K key)
{{
    std::pair<bool, size_t> result = {{false, 0}};
    size_t                  i      = 1;
    size_t                  n      = it_end - it_begin;
    while(i <= n){{
        const auto& curr        = it_begin[i - 1];
        const auto& curr_bounds = curr.bounds;
        if(key < curr_bounds.lower_bound){{
            i = 2 * i;
        }}else if(key > curr_bounds.upper_bound){{
            i = 2 * i + 1;
        }}else{{
            result.first = true; result.second = i - 1;
            break;
        }}
    }}
    return result;
}}

static const Segment_with_value<char32_t, uint64_t> categories_table[] = {{
{0}
}};

static constexpr size_t num_of_elems_in_categories_table = {1};

uint64_t get_categories_set(char32_t c)
{{
    auto t = knuth_find(categories_table,
                        categories_table + num_of_elems_in_categories_table,
                        c);
    return t.first ? categories_table[t.second].value :
                     1ULL << static_cast<uint64_t>(Category::Other);
}}
)~";

std::string show_table()
{
    std::string result;
    auto   t                 = create_classification_table(table);
    size_t num_of_elems      = t.size();

    std::vector<std::string> elems;
    for(const auto& e : t){
        elems.push_back(show_table_elem(e));
    }

    Format f;
    f.indent                 = 4;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 2;

    auto table_body          =  string_list_to_columns(elems, f);
    result                   =  fmt::format(table_fmt, table_body, num_of_elems);
    return result;
}

#ifdef DEBUG
void print_filled_table(){
    for(const auto e : table){
        std::string s = show_char32(e.first);
        printf("{%s, %d} \n", s.c_str(), e.second);
    }
    putchar('\n');
}

void print_vector(const std::vector<std::pair<char32_t, uint16_t>>& v){
    for(const auto e : v){
        std::string s = show_char32(e.first);
        printf("{%s, %d} \n", s.c_str(), e.second);
    }
    putchar('\n');
}

void print_grouped_vector(const SegmentsV<char32_t, uint16_t>& gv){
    for(const auto e : gv){
        auto s = show_table_elem(e);
        printf("%s \n",s.c_str());
    }
    putchar('\n');
}

void print_permutation_node(const Permutation_node& node){
    printf("{index = %zu, left = %zu, right = %zu, parent = %zu}",
           node.index, node.left, node.right, node.parent);
}

void print_permutation_tree(const Permutation_tree& pt){
    for(const auto& node : pt){
        print_permutation_node(node);
        putchar('\n');
    }
}

void print_permutation(const Permutation& p){
    for(auto i : p){
        printf("%zu ", i);
    }
    putchar('\n');
}
#endif

int main(){
    fill_table();
#ifdef DEBUG
    puts("Table as map:");
    print_filled_table();
    puts("*******************************************************************");
    auto v  = map_as_vector(table);
    puts("Table as vector:");
    print_vector(v);
    puts("*******************************************************************");
    printf("Number of elements in vector is: %zu.\n", v.size());
    puts("*******************************************************************");
    auto gv = group_pairs(v);
    puts("Grouped pairs: ");
    print_grouped_vector(gv);
    puts("*******************************************************************");
    printf("Number of grouped pairs is: %zu.\n", gv.size());
    puts("*******************************************************************");
    puts("Permutation tree:");
    auto pt = create_permutation_tree(gv.size());
    print_permutation_tree(pt);
    puts("*******************************************************************");
    auto p = permutation_tree_to_permutation(pt);
    puts("Permutation: ");
    print_permutation(p);
    puts("*******************************************************************");
    puts("Result of function create_permutation:");
    auto cp = create_permutation(gv.size());
    print_permutation(cp);
    puts("*******************************************************************");
    auto t = create_classification_table(table);
    puts("Final classification table is: ");
    print_grouped_vector(t);
    puts("*******************************************************************");
    print();
#else
    auto table_as_string = show_table();
    puts(table_as_string.c_str());
#endif
    return 0;
}
