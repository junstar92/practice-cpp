#include <iostream>

#include "async_vs_thread.h"
#include "async.h"
#include "shared_future.h"
#include "execption.h"
#include "notification.h"

int main()
{
    async_vs_thread::exam();

    async::async_basic();
    async::start_policy();
    async::fire_and_forget();
    async::concurrent_calculation();
    async::passing_args_to_async();
    async::package_task();
    async::promise_and_future();

    shared_future::shared_future();
    shared_future::shared_future_from_future();

    exception::except_promise();

    notification::notification();
}