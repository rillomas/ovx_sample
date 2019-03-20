#include <VX/vx.h>
#include <VX/vxu.h>
#include <iostream>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
  spdlog::info("Initializing resources");
  vx_context context = vxCreateContext();
  vx_graph graph = vxCreateGraph(context);
  spdlog::info("Releasing resources");
  vxReleaseGraph(&graph);
  vxReleaseContext(&context);
  return 0;
}
