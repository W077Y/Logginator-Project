#include <array>
#include <logginator.hpp>
//

#include <iostream>

int main()
{
  std::cout << "Hello, World!" << std::endl;

  logginator::detail::downsampler_t sam1;
  logginator::detail::downsampler_t sam2 = {2};
  logginator::detail::downsampler_t sam3 = {3};

  sam1.set_trg(2);
  for (int i = 0; i< 10; i++)
  {
    std::cout << i << ": " << sam1.is_ready() << ", " << sam2.poll() << ", " << sam3.poll() << std::endl;
    sam1.tick();
  }

  std::cout << "Bye bye, World!" << std::endl;
  return 0;
}
