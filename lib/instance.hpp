#pragma once

#include "info.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

namespace knapsack
{

typedef int_fast64_t Profit;
typedef int_fast64_t Weight;
typedef int_fast64_t ItemIdx;
typedef int_fast64_t ItemPos;
typedef int_fast64_t StateIdx;
typedef int_fast64_t Label;

class Solution;
typedef int64_t PartSol1;
typedef int64_t PartSol2;
class PartSolFactory1;
class PartSolFactory2;

struct Item
{
    Item(ItemIdx j, Weight w, Profit p): j(j), w(w), p(p) { }
    Item(ItemIdx j, Weight w, Profit p, Label l): j(j), w(w), p(p), l(l) { }
    Item& operator=(const Item& item)
    {
        if (this != &item) {
            j = item.j;
            w = item.w;
            p = item.p;
            l = item.l;
        }
        return *this;
    }
    ItemIdx j = -1; // Index, 0 <= j < n, must be different for each item
    Weight  w = -1; // Weight, w >= 0
    Profit  p = -1; // Profit, p >= 0
    Label   l = -1; // Label, this parameter is not used in this package
};

struct Interval
{
    ItemPos f;
    ItemPos l;
};

class Instance
{

public:

    /**
     * Constructors
     */

    /**
     * Manual constructor.
     */
    Instance(ItemIdx n, Weight c);
    void add_item(Weight w, Profit p);
    void add_item(Weight w, Profit p, Label l);
    void add_items(const std::vector<std::pair<Weight, Profit>>& wp);
    void update_item(ItemIdx j, Weight w, Profit p);
    void update_item(ItemIdx j, Weight w, Profit p, Label l);
    void update_profit(ItemIdx j, Profit p);
    void update_weight(ItemIdx j, Weight w);

    /**
     * Create instance from file.
     */
    Instance(boost::filesystem::path filename);

    /**
     * Copy constructor
     */
    Instance(const Instance& ins);

    ~Instance();


    /**
     * Getters
     */

    inline std::string name()   const { return name_; }
    inline std::string format() const { return format_; }

    inline ItemIdx total_item_number() const { return items_.size(); }
    inline Weight  total_capacity()    const { return c_orig_; }
    inline const Item& item(ItemIdx j) const { assert(j >= 0 && j < total_item_number()); return items_[j]; }

    const Solution* optimal_solution() const { return sol_opt_; }
    Profit optimum() const;


    /*
     * Sort items according to non-increasing profit-to-weight ratio.
     */
    void sort();
    bool sorted() const { return sorted_; }

    /**
     * Sort items partially according to non-increasing profit-to-weight
     * ratio, i.e. the break item is the same as if the items were fully
     * sorted. This is achieved with a quick-sort like algorithm as
     * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
     * (Pisinger, 1997).
     */
    void sort_partially();
    void sort_right(Profit lb);
    void sort_left(Profit lb);
    ItemPos int_right_size() const { return int_right_.size(); }
    ItemPos int_left_size()  const { return int_left_.size();  }
    ItemPos first_sorted_item() const { return s_; }
    ItemPos last_sorted_item()  const { return t_; }
    bool break_item_found() const { return (b_ != -1); }

    /**
     * Apply variable reduction. See "Knapsack Problem", Chap 3.2:
     * "Variable Reduction" (Kellerer, 2004) for more details.
     * The first one uses Dembo and Hammer UB and only requires a partial
     * sorting of the items.
     * The second one uses Dantzig UB and requires a complete sorting of the
     * items.
     * Fixed items are moved at the beginning or at the end of the item vector
     * and the item number and the capacity are updated.
     * Note that the break item may change.
     * The solution with fixed variables is returned by the method
     * reduced_solution().
     * After a variable reduction, if the capacity of the instance is negative,
     * then the lower bound is optimal. Otherwise, the optimal value is the max
     * between the lower bound and the optimal value of the reduced solution.
     */
    void reduce1(Profit lb, bool verbose = false);
    void reduce2(Profit lb, bool verbose = false);
    const Solution* reduced_solution() const { return sol_red_; }

    inline ItemIdx item_number() const { return l_-f_+1; }
    inline ItemPos first_item()  const { return f_; }
    inline ItemPos last_item()   const { return l_; }
    Weight capacity() const;

    /**
     * Reduce item f..j-1, and add them to the reduced solution
     */
    void set_first_item(ItemPos k);
    /**
     * Reduce items j+1..l (there are not added in the reduced solution)
     */
    void set_last_item(ItemPos k);

    void fix(PartSolFactory1 bsolf, PartSol1 bsol);
    void fix(PartSolFactory2 bsolf, PartSol2 bsol);

    /**
     * Create an instance with capacitiy and weights divided, keeping the
     * floor (resp. the ceiling).
     */
    void divide_weights_floor(Weight divisor);
    void divide_weights_ceil(Weight divisor);
    void divide_profits_floor(Profit divisor);
    void divide_profits_ceil(Profit divisor);

    void surrogate(Weight multiplier, ItemIdx bound, ItemPos first);
    void surrogate(Weight multiplier, ItemIdx bound);

    const Solution* break_solution()   const { return sol_break_; }
    ItemPos break_item()     const { assert(b_ >= first_item() && b_ <= last_item() + 1); return b_; }
    Profit  break_profit()   const;
    Weight  break_weight()   const;
    Weight  break_capacity() const;

    const Item& max_weight_item();
    const Item& max_profit_item();
    const Item& max_efficiency_item();

    const Item& isum(ItemPos j) const { assert(sorted()); return isum_[j]; }
    ItemPos ub_item(Item item) const;


    /**
     * Return the profit of the certificate file.
     */
    Profit check(boost::filesystem::path cert_file);

    bool check_opt(Profit p) const;
    bool check_sopt(const Solution& sol) const;
    bool check_ub(Profit p) const;
    bool check_lb(Profit p) const;
    bool check_sol(const Solution& sol) const;

    /**
     * return "LB XXXX GAP XXXX" if optimal_solution() != NULL,
     *        "LB XXXX" otherwise.
     */
    std::string print_lb(Profit lb) const;
    std::string print_ub(Profit ub) const;
    std::string print_opt(Profit opt) const;

private:

    /*
     * Methods
     */

    void read_standard(std::stringstream& data);
    void read_subsetsum_standard(std::stringstream& data);
    void read_pisinger(std::stringstream& data);
    void read_standard_solution(boost::filesystem::path filepath);

    ItemPos partition(ItemPos f, ItemPos l);
    bool check();
    inline void swap(ItemPos j, ItemPos k) { Item tmp = items_[j]; items_[j] = items_[k]; items_[k] = tmp; };
    void update_isum();
    void compute_break_item();
    void compute_max_items();
    /*
     * Remove items which weight is greater than the updated capacity
     */
    void remove_big_items();


    /*
     * Attributes
     */

    std::string name_;
    std::string format_;

    std::vector<Item> items_;
    Weight c_orig_;
    Solution* sol_opt_ = NULL; // Optimal solution
    int version_ = 0;

    Item j_wmax_ = {-1, -1, -1}; // Max weight item
    Item j_wmin_ = {-1, c_orig_+1, -1}; // Min weight item
    Item j_pmax_ = {-1, -1, -1}; // Max profit item
    Item j_emax_ = {-1, 0, -1};  // Max efficiency item;
    int max_version_ = 0;


    ItemPos f_, l_, s_, t_;
    bool sorted_ = false;
    std::vector<Interval> int_right_, int_left_;

    Solution* sol_red_   = NULL; // Reduced solution
    Solution* sol_break_ = NULL; // Break solution
    ItemPos b_ = -1; // Break item
    std::vector<Item> isum_;
    bool sol_red_opt_ = false;
};

std::ostream& operator<<(std::ostream &os, const Item& item);
std::ostream& operator<<(std::ostream &os, const Interval& interval);
std::ostream& operator<<(std::ostream &os, const Instance& instance);

}
