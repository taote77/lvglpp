#ifndef LV_BASE_MODEL_H
#define LV_BASE_MODEL_H

#include <vector>

namespace lvglpp {
namespace widgets {
template <class T>
class BaseModel
{
public:
    BaseModel() = default;

    virtual ~BaseModel() = default;

    explicit BaseModel(const std::vector<T> &data);

    bool append(const T &data);

    bool insert(int index, const T &data);

    bool remove(int index);

    void clear();

    int count() const;

    const T &getItem(int index) const;

    bool changed(int index, const T &data);

private:
    std::vector<T> model_data_;
};

template <class T>
BaseModel<T>::BaseModel(const std::vector<T> &data) : model_data_(data)
{
}

template <class T>
bool BaseModel<T>::append(const T &data)
{
    model_data_.push_back(data);
    return true;
}

template <class T>
bool BaseModel<T>::insert(int index, const T &data)
{
    if (index < 0 || index >= model_data_.size()) {
        return false;
    }
    model_data_.insert(model_data_.begin() + index, data);
    return true;
}

template <class T>
bool BaseModel<T>::remove(int index)
{
    if (index < 0 || index >= model_data_.size()) {
        return false;
    }
    model_data_.erase(model_data_.begin() + index);
    return true;
}

template <class T>
void BaseModel<T>::clear()
{
    model_data_.clear();
}

template <class T>
int BaseModel<T>::count() const
{
    return model_data_.size();
}

template <class T>
const T &BaseModel<T>::getItem(int index) const
{
    return model_data_[index];
}

template <class T>
bool BaseModel<T>::changed(int index, const T &data)
{
    if (index < 0 || index >= model_data_.size()) {
        return false;
    }
    model_data_[index] = data;
    return true;
}
} // namespace widgets
} // namespace lvglpp

#endif // LV_BASE_MODEL_H
