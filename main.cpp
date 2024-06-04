#include <string>
#include <memory>
#include <iostream>
#include <utility>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sorted.h"
#include "unsorted.h"
#include "lazy.h"

struct ColorManager
{
  static constexpr const char *reset = "\033[0m";
  static constexpr const char *red = "\033[0;31m";
  static constexpr const char *green = "\033[0;32m";
  static constexpr const char *yellow = "\033[0;33m";
  static constexpr const char *blue = "\033[0;34m";
  static constexpr const char *magenta = "\033[0;35m";
  static constexpr const char *cyan = "\033[0;36m";
  static constexpr const char *white = "\033[0;37m";

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
#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
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
};

class Context
{
  using Heap = MergeableHeap<int>;
  using Color = ColorManager::Color;

  enum class Action
  {
    invalid = 0,
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
  explicit Context() : heap1(nullptr), heap2(nullptr) {}

  ~Context()
  {
    if (heap1)
      delete heap1;
    if (heap2)
      delete heap2;
    ColorManager::set_color(ColorManager::Color::reset);
  }

  void init()
  {
    print_welcome();
    init_sorted();
    execute_make(heap1);
    execute_make(heap2);
    read_file("input.txt");
  }

  void run()
  {
    while (true)
    {
      Heap *heap = nullptr;
      do
      {
        heap = prompt_heap();
      } while (heap == nullptr);

      Action action = Action::invalid;
      do
      {
        action = prompt_action();
      } while (action == Action::invalid);

      execute_action(heap, action);
    }
  }

private:
  void print_welcome()
  {
    ColorManager::set_color(Color::green);
    std::cout << "************************************************************\n"
              << "*                                                          *\n"
              << "*       " << ColorManager::cyan << "~ Welcome to a world of wonder and magic! ~ " << ColorManager::green << "       *\n"
              << "*                                                          *\n"
              << "************************************************************\n";
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
      std::cout << number << ", ";
    }
    std::cout << "\b\b.\n";
  }

  void read_file(const std::string &filepath)
  {
    std::ifstream file(filepath);
    if (!file.is_open())
    {
      throw std::runtime_error("Could not open file " + filepath);
    }

    std::string line1, line2;
    // Read the first two lines
    if (std::getline(file, line1) && std::getline(file, line2))
    {
      // Check if there's more data in the file
      if (!file.eof())
      {
        std::cout << "More than two lines where given in the input file.\n"
                  << "Ignoring the third line and below.\n";
      }
      // Process the lines
      std::cout << "heap1: ";
      read_line(heap1, line1);
      std::cout << "heap2: ";
      read_line(heap2, line2);
    }
    else
    {
      throw std::runtime_error("Could not read data from file.");
    }
  }

  void init_sorted()
  {
    ColorManager::set_color(Color::yellow);
    std::cout << "Use sorted linked-lists? (y/n) ";
    std::cin >> input;
    sorted = input == "y";

    if (sorted)
    {
      std::cout << "using sorted linked-lists!\n";
    }
    else
    {
      std::cout << "using unsorted linked-lists!\n";
    }
  }

  Heap *prompt_heap()
  {
    std::cout << "Choose a heap: (1/2) ";
    std::cin >> input;
    if (input == "1")
    {
      std::cout << "heap1 was chosen.\n";
      return heap1;
    }
    else if (input == "2")
    {
      std::cout << "heap2 was chosen.\n";
      return heap2;
    }
    std::cout << "please enter 1 or 2.\n";
    return nullptr;
  }

  Action prompt_action()
  {
    std::cout << "Choose an action: make-(h)eap, (i)nsert, (m)inimum, (e)xtract-min, me(r)ge, (p)rint ";
    std::cin >> input;
    if (input.size() != 1)
    {
      std::cout << "please choose one letter from {h, i, m, e, r, p}.\n";
      return Action::invalid;
    }

    switch (input[0])
    {
    case 'h':
    {
      std::cout << "a creation of a new heap was requested.\n";
      return Action::make_heap;
    }
    case 'i':
    {
      std::cout << "an insertion was requested.\n";
      return Action::insert;
    }
    case 'm':
    {
      std::cout << "a minimum was requested.\n";
      return Action::minimum;
    }
    case 'e':
    {
      std::cout << "an extraction was requested.\n";
      return Action::extract;
    }
    case 'r':
    {
      std::cout << "a merge was requested.\n";
      return Action::merge;
    }
    case 'p':
    {
      std::cout << "a printing was requested.\n";
      return Action::print;
    }
    }

    std::cout << "please choose a letter from {i, m, e, r, p}.\n";
    return Action::invalid;
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
    std::cout << "Chosen heap is now: ";
    heap->print();
  }

  void execute_make(Heap *&heap)
  {
    delete heap;
    heap = sorted ? static_cast<Heap *>(new SortedLinkedHeap<int>()) : static_cast<Heap *>(new UnsortedLinkedHeap<int>());
  }

  void execute_insert(Heap *heap)
  {
    std::cout << "Executing insertion. Enter a number to insert: ";
    std::cin >> input;
    while (true)
    {
      try
      {
        heap->insert(std::stoi(input));
        std::cout << "\binsertion succeeded!\n";
        return;
      }
      catch (...)
      {
        std::cout << "please enter a valid number: ";
        std::cin >> input;
      }
    }
  }

  void execute_minimum(Heap *heap)
  {
    auto res = heap->minimum();
    if (!res)
    {
      std::cout << "The chosen heap is empty, there is no minimum!\n";
    }
    else
    {
      std::cout << "The minimum of the chosen heap is " << *res << ".\n";
    }
  }

  void execute_extract(Heap *heap)
  {
    auto res = heap->extract_min();
    if (!res)
    {
      std::cout << "The chosen heap is empty, there is nothing to extract!\n";
    }
    else
    {
      std::cout << "The number " << *res << " was extracted from the chosen heap.\n";
    }
  }

  void execute_merge()
  {
    std::cout << "Executing merge.\n";
    heap1->merge(*heap2);
  }

  void execute_print(Heap *heap)
  {
    std::cout << "Printing chosen heap: ";
    heap->print();
  }
};

int main()
{
  Context context{};

  try
  {
    context.init();
  }
  catch (std::exception const &e)
  {
    std::cout << "failed to read file: " << e.what() << ".\nTerminating program.\n";
    return 0;
  }

  context.run();
}