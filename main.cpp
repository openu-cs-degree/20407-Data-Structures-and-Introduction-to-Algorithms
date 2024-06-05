#include <string>
#include <memory>
#include <iostream>
#include <utility>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "sorted.h"
#include "unsorted.h"
#include "lazy.h"

class Game
{
  class ColorManager
  {
#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define COLORS_AVAILABLE 1
#else
#define COLORS_AVAILABLE 0
#endif

    static constexpr const char *reset = "\033[0m";
    static constexpr const char *red = "\033[0;31m";
    static constexpr const char *green = "\033[0;32m";
    static constexpr const char *yellow = "\033[0;33m";
    static constexpr const char *blue = "\033[0;34m";
    static constexpr const char *magenta = "\033[0;35m";
    static constexpr const char *cyan = "\033[0;36m";
    static constexpr const char *white = "\033[0;37m";

  public:
    enum class Color
    {
      reset,
      red,
      green,
      yellow,
      blue,
      magenta,
      cyan,
      white
    };

    static void set_color([[maybe_unused]] Color color = Color::reset)
    {
#if COLORS_AVAILABLE
      switch (color)
      {
      case Color::red:
        std::cout << red;
        break;
      case Color::green:
        std::cout << green;
        break;
      case Color::yellow:
        std::cout << yellow;
        break;
      case Color::blue:
        std::cout << blue;
        break;
      case Color::magenta:
        std::cout << magenta;
        break;
      case Color::cyan:
        std::cout << cyan;
        break;
      case Color::reset:
        std::cout << reset;
        break;
      case Color::white:
        std::cout << white;
        break;
      }
#endif
    }

    static void set_border()
    {
      set_color(Color::green);
    }

    static void set_text()
    {
      set_color(Color::reset);
    }

    static void set_instruction()
    {
      set_color(Color::yellow);
    }

    static void set_error()
    {
      set_color(Color::red);
    }

    static void set_title()
    {
      set_color(Color::cyan);
    }

    static void set_info()
    {
      set_color(Color::magenta);
    }
  };

  using Heap = MergeableHeap<int>;
  using Color = ColorManager::Color;

  enum class Action
  {
    extract = 'e',
    insert = 'i',
    make_heap = 'h',
    minimum = 'm',
    print = 'p',
    merge = 'r',
  };

private:
  bool sorted = false;
  std::string input{};
  Heap *heap1;
  Heap *heap2;

public:
  explicit Game() : heap1(nullptr), heap2(nullptr) {}

  ~Game()
  {
    if (heap1)
      delete heap1;
    if (heap2)
      delete heap2;
    ColorManager::set_color();
  }

  void init()
  {
    print_welcome();
    init_sorted();
    init_heaps();
  }

  void run()
  {
    while (true)
    {
      auto heap = prompt_heap();
      if (!heap)
      {
        break;
      }
      Action action = prompt_action();
      execute_action(*heap, action);
    }
    print_goodbye();
  }

private:
  template <const char *text>
  constexpr void print_line() const
  {
    static constexpr size_t line_length = 56; // 60 including borders
    const size_t text_length = std::strlen(text);
    static_assert(text_length <= line_length, "Text exceeds line length!");

    ColorManager::set_border();
    std::cout << "* ";
    ColorManager::set_text();
    std::cout << text;
    ColorManager::set_border();
    for (size_t i = 0; i < line_length - text_length; ++i)
    {
      std::cout << ' ';
    }
    std::cout << " *\n";
  }

  template <const char *...texts>
  constexpr void print_lines(const bool print_begin_and_end = true) const
  {
    if (print_begin_and_end)
    {
      print_begin();
    }

    ((print_line<texts>()), ...);

    if (print_begin_and_end)
    {
      print_end();
    }
  }

  constexpr void print_begin() const
  {
    ColorManager::set_border();
    std::cout << "************************************************************\n"
              << "*                                                          *\n";
  }

  constexpr void print_end() const
  {
    ColorManager::set_border();
    std::cout << "*                                                          *\n"
              << "************************************************************\n";
  }

  constexpr void print_welcome() const
  {
    auto magic = []() -> std::string
    {
#if COLORS_AVAILABLE
      return "\033[0;31mm\033[0;32ma\033[0;33mg\033[0;34mi\033[0;35mc\033[0;36m!";
#else
      return "magic!";
#endif
    };

    static constexpr const char empty[] = "";
    static constexpr const char line1[] = "I am Wizzy The Wizard, and I will guide you through a";
    static constexpr const char line2[] = "magical journey through the world of heaps.";
    static constexpr const char line3[] = "Our story involves two main characters: heap1 and heap2.";
    static constexpr const char line4[] = "Let's get to know our characters!";
    static constexpr const char line5[] = "hint: you can terminate the program anytime by pressing";
    static constexpr const char line6[] = "Ctrl+C.";

    print_begin();
    std::cout << "*       ";
    ColorManager::set_title();
    std::cout << "~ Welcome to a world of wonder and " + magic() + " ~ ";
    ColorManager::set_border();
    std::cout << "       *\n";
    print_lines<empty, line1, line2, empty, line3, line4, empty, line5, line6>(false);
    print_end();
  }

  constexpr void print_goodbye()
  {
    static constexpr const char line1[] = "And so our magical journey comes to an end...";
    static constexpr const char line2[] = "Thank you for joining me on this adventure!";
    static constexpr const char line3[] = "I hope you had as much fun as I did!";
    static constexpr const char line4[] = "Goodbye, dear reader!";
    static constexpr const char empty[] = "";

    print_begin();
    print_lines<line1, line2, line3, empty, line4, empty>(false);
    std::cout << "*       ";
    ColorManager::set_title();
    std::cout << "       ~ Wizzy The Wizzard (c) 2024 ~       ";
    ColorManager::set_border();
    std::cout << "       *\n";
    print_end();
  }

  void read_line(Heap *heap, const std::string &str)
  {
    static constexpr char delimiter = ' ';
    std::stringstream ss(str);
    std::string number;
    while (std::getline(ss, number, delimiter))
    {
      int res = std::stoi(number);
      heap->insert(res);
    }
  }

  void read_file(const std::string &filepath)
  {
    std::ifstream file(filepath);
    if (!file.is_open())
    {
      throw std::runtime_error("The land of " + filepath + " does not appear in my maps!");
    }

    std::string line1, line2;
    // Read the first two lines, ignore the rest
    if (std::getline(file, line1) && std::getline(file, line2))
    {
      read_line(heap1, line1);
      read_line(heap2, line2);
    }
    else
    {
      throw std::runtime_error("Beep beep boop boop... I can't read this file!");
    }
  }

  void init_sorted()
  {
    prompt("Do you want the heaps to be sorted? (y/n) ");

    sorted = input == "y";

    print_begin();
    if (sorted)
    {
      static constexpr const char line[] = "Our heaps love being sorted, of course!";
      print_line<line>();
    }
    else
    {
      static constexpr const char line[] = "Our heaps are adventurous and unsorted!";
      print_line<line>();
    }
    static constexpr const char line1[] = "And they live in the magical world of... uh...";
    static constexpr const char line2[] = "Oh no! I forgot the name of their world... a disaster!";
    static constexpr const char line3[] = "";
    static constexpr const char line4[] = "Dear reader, can you help me remember where they live?";
    print_lines<line1, line2, line3, line4>(false);
    print_end();
  }

  void init_heaps()
  {
    execute_make(heap1, false);
    execute_make(heap2, false);

    while (true)
    {
      try
      {
        prompt("Enter a file path to read the heaps from: ");
        read_file(input);
        break;
      }
      catch (std::exception const &e)
      {
        ColorManager::set_error();
        std::cout << "Oh no! What a shame!\n"
                  << e.what() << "\n"
                  << "Let's try again:\n";
      }
    }

    static constexpr const char line1[] = "Yayyyy, how wonderful indeed!";
    static constexpr const char line2[] = "Our heaps have been successfully initializd!";
    static constexpr const char line3[] = "";
    static constexpr const char line4[] = "(...that was not a typo, I swear)";
    static constexpr const char line5[] = "(I may be a wizard, but I could not rhyme \"indeed\"...)";
    static constexpr const char line6[] = "(And don't you dare judge me for that!)";
    static constexpr const char line7[] = "";
    static constexpr const char line8[] = "Where were we? Oh yes, the heaps!";
    static constexpr const char line9[] = "Let's have a glance at their beauty and splendor!";

    print_lines<line1, line2, line3, line4, line5, line6, line7, line8, line9>();

    execute_print(heap1, false);
    execute_print(heap2, false);
  }

  constexpr void prompt(const char *message)
  {
    ColorManager::set_instruction();
    std::cout << message;
    ColorManager::set_text();
    std::cin >> input;
  }

  std::optional<Heap *> prompt_heap()
  {
    static constexpr const char line1[] = "And to my next magic trick, I need a volunteering heap!";
    static constexpr const char line2[] = "Which heap is brave enough to go on a spooky quest?!";
    print_lines<line1, line2>();

    do
    {
      prompt("Choose a heap: (1/2/exit) ");

      if (input == "1")
      {
        return heap1;
      }
      else if (input == "2")
      {
        return heap2;
      }
      else if (input == "exit")
      {
        return std::nullopt;
      }
      ColorManager::set_error();
      std::cout << "please enter 1 or 2 (or exit if you're not feeling magical anymore ;-;).\n";
    } while (true);
  }

  Action prompt_action()
  {
    static constexpr const char line1[] = "A magnificent choice, I couldn't agree more!";
    static constexpr const char line2[] = "Now, what shall be this brave heap's quest?";
    print_lines<line1, line2>();
    do
    {
      prompt("Choose an action: make-(h)eap, (i)nsert, (m)inimum, (e)xtract-min, me(r)ge, (p)rint ");

      if (input.size() != 1)
      {
        ColorManager::set_error();
        std::cout << "please choose one letter from {h, i, m, e, r, p}.\n";
        continue;
      }

      switch (input[0])
      {
      case 'h':
        return Action::make_heap;
      case 'i':
        return Action::insert;
      case 'm':
        return Action::minimum;
      case 'e':
        return Action::extract;
      case 'r':
        return Action::merge;
      case 'p':
        return Action::print;
      }

      ColorManager::set_error();
      std::cout << "please choose a letter from {i, m, e, r, p}.\n";
    } while (true);
  }

  void execute_action(Heap *heap, Action action)
  {
    switch (action)
    {
    case Action::make_heap:
    {
      execute_make(heap);
      break;
    }
    case Action::insert:
    {
      execute_insert(heap);
      break;
    }
    case Action::minimum:
    {
      execute_minimum(heap);
      break;
    }
    case Action::extract:
    {
      execute_extract(heap);
      break;
    }
    case Action::merge:
    {
      execute_merge();
      break;
    }
    case Action::print:
    {
      execute_print(heap);
      return; // no need to print the heap twice
    }
    default:
      std::unreachable();
    }
    execute_print(heap, false);
  }

  void execute_make(Heap *&heap, bool print_jizzly_joo = true)
  {
    if (print_jizzly_joo)
    {
      static constexpr const char line1[] = "A new heap is born! How exciting!";
      static constexpr const char line2[] = "From the ashes of the old, a new heap now rises!!!";
      print_lines<line1, line2>();
    }

    delete heap;
    heap = sorted ? static_cast<Heap *>(new SortedLinkedHeap<int>()) : static_cast<Heap *>(new UnsortedLinkedHeap<int>());
  }

  void execute_insert(Heap *heap)
  {
    static constexpr const char line1[] = "A brave choice, indeed!";
    static constexpr const char line2[] = "What number shall we insert into the heap?";
    print_lines<line1, line2>();

    while (true)
    {
      prompt("Enter a number to insert: ");

      try
      {
        heap->insert(std::stoi(input));
        ColorManager::set_info();
        std::cout << "\binsertion succeeded!\n";
        return;
      }
      catch (...)
      {
        ColorManager::set_error();
        std::cout << "That's not a number, dear reader! Please try again.\n";
      }
    }
  }

  void execute_minimum(Heap *heap)
  {
    static constexpr const char line1[] = "A wise choice, indeed!";
    static constexpr const char line2[] = "Let's see what mysteries are held in our brave heap!";
    print_lines<line1, line2>();

    auto res = heap->minimum();
    if (!res)
    {
      ColorManager::set_error();
      std::cout << "The chosen heap is empty, there is no minimum!\n";
    }
    else
    {
      ColorManager::set_info();
      std::cout << "The minimum of the chosen heap is " << *res << ".\n";
    }
  }

  void execute_extract(Heap *heap)
  {
    static constexpr const char line1[] = "Why yes, a very wise choice alright.";
    static constexpr const char line2[] = "Let's extract the minimum and throw it into the night!";
    print_lines<line1, line2>();

    auto res = heap->extract_min();
    if (!res)
    {
      ColorManager::set_error();
      std::cout << "The chosen heap is empty, there is nothing to extract!\n";
    }
    else
    {
      ColorManager::set_info();
      std::cout << "The number " << *res << " was extracted from the chosen heap.\n";
    }
  }

  void execute_merge()
  {
    static constexpr const char line1[] = "A merge was requested, wow, splendid!";
    print_lines<line1>();

    heap1->merge(*heap2);
  }

  void execute_print(Heap *heap, bool print_jizzly_joo = true)
  {
    if (print_jizzly_joo)
    {
      static constexpr const char line1[] = "Brace yourselves, let's reveal the secrets of the heap!";
      static constexpr const char line2[] = "Here it comes, I can feel it...";
      static constexpr const char line3[] = "Just one moment...";
      static constexpr const char line4[] = "";
      static constexpr const char line5[] = "...There!";
      print_lines<line1, line2, line3, line4, line5>();
    }

    ColorManager::set_info();
    if (heap == heap1)
    {
      std::cout << "heap1: ";
    }
    else
    {
      std::cout << "heap2: ";
    }
    ColorManager::set_text();
    heap->print();
    std::cout << '\n';
  }
};

int main()
{
  Game game{};
  game.init();
  game.run();
}