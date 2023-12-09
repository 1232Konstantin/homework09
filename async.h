#ifndef ASSINC_H
#define ASSINC_H

#include <cstddef>
#include <memory>
#include <thread>
#include "executorthread.h"
#include "servicethreads.h"
#include "handle.h"


namespace async {

using handle_t = void *;

handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}










#endif // ASSINC_H
