#include "solution.hpp"

Solution::Solution(const Instance& instance): instance_(instance),
	x_(std::vector<int>(instance.total_item_number(), 0)) { }

Solution::Solution(const Solution& solution):
	instance_(solution.instance()), k_(solution.item_number()),
	p_(solution.profit()), w_(solution.weight()), x_(solution.data())
{ }

Solution& Solution::operator=(const Solution& solution)
{
	if (this != &solution) {
		k_ = solution.item_number();
		p_ = solution.profit();
		w_ = solution.weight();
		x_ = solution.data();
	}
	return *this;
}

int Solution::contains(ItemPos i) const
{
	assert(i >= 0 && i < instance().total_item_number());
	assert(instance().item(i).i >= 0 && instance().item(i).i < instance().total_item_number());
	return x_[instance().item(i).i];
}

void Solution::set(ItemPos i, int b)
{
	assert(b == 0 || b == 1);
	assert(i >= 0 && i < instance().total_item_number());
	assert(instance().item(i).i >= 0 && instance().item(i).i < instance().total_item_number());
	if (contains(i) == b)
		return;
	if (b) {
		p_ += instance().item(i).p;
		w_ += instance().item(i).w;
		k_++;
	} else {
		p_ -= instance().item(i).p;
		w_ -= instance().item(i).w;
		k_--;
	}
	x_[instance().item(i).i] = b;
}

bool Solution::update(const Solution& sol)
{
	if (sol.profit() <= profit())
		return false;
	*this = sol;
	return true;
}

std::ostream& operator<<(std::ostream& os, const Solution& solution)
{
	const Instance& instance = solution.instance();
	for (ItemPos i=0; i<instance.total_item_number(); ++i)
		os << solution.data()[i] << std::endl;
	return os;
}

