#include <shared_mutex>
#include <mutex>
#include <iterator>

template <template <typename> class Container, typename T>
class ConcurrentContainerFacade
{
private:
    mutable std::shared_mutex mutex;
    Container<T> containerImpl;

public:
    void insert(auto&& container)
    {
        std::unique_lock lock{mutex};
        containerImpl.insert(std::begin(container), std::end(container));
    }

    auto size() const
    {
        std::shared_lock lock{mutex};
        return containerImpl.size();
    }
};
