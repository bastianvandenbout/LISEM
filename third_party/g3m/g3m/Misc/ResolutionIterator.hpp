//
// Created by robert on 20.08.16.
//

#ifndef GROUNDWATER_RESOLUTIONITERATOR_HPP
#define GROUNDWATER_RESOLUTIONITERATOR_HPP

/**
 * Use of iterator to iterate over a 0.5° grid and apply a function for all 5 min nodes inside each:
 * for(const auto gridcell& : iterator) {
 *      container[gridcell.id] = gridcell.map([] lambda())
 * }
 */


using NodeVector = std::shared_ptr<std::vector<std::unique_ptr<NodeInterface>>>;
using LookupTable = std::unordered_map<int, vector<int>>;
using IDTable = std::unordered_map<int,int>;
using Item = NodeInterface*;
using size_type = unsigned long;
using Step = unsigned long;
using AssociatedNodes = std::vector<Item>;
//using namespace std;


class FiveDegreeToZeroFiveItem {
 private:
    AssociatedNodes nodes;
    static AssociatedNodes getData(const NodeVector& vector, const LookupTable& lu_table,
                   const IDTable& id_table, const size_type& pos){
        AssociatedNodes out;
        std::vector<int> tmp;
        try{tmp = lu_table.at(pos);}
        catch (const std::out_of_range& oor){return out;} //No 5min nodes in 5min cell

        int i = 0;
        for (int id : tmp) {
            i = id_table.at(id);
            out.push_back(vector->at(i).get());
        }
        return out;
    }

    template <
        class ElemT,
        class FunctionT,
        class T = decltype(std::declval<FunctionT>()(std::declval<ElemT>())),
        typename = typename std::enable_if<!std::is_void<T>::value>::type
    >
    const T iterate_and_sum(FunctionT fun) const{
        T val;
        for (int j = 0; j < nodes.size(); ++j) {
            val += fun(nodes.at(j));
        }
        return val;
    }

    template <
        class ElemT,
        class FunctionT,
        class T = decltype(std::declval<FunctionT>()(std::declval<ElemT>())),
        typename = typename std::enable_if<std::is_void<T>::value>::type
    >
    const T iterate(FunctionT fun) const{
        for (int j = 0; j < nodes.size(); ++j) {
            fun(nodes.at(j));
        }
    }

 public:
    size_type id;
    FiveDegreeToZeroFiveItem(NodeVector vector, LookupTable lu_table,
                             IDTable id_table, size_type pos)
        : nodes(getData(vector, lu_table, id_table, pos)), id(pos) {}
    FiveDegreeToZeroFiveItem(AssociatedNodes nodes, size_type id) : nodes(nodes), id(id) {}
    FiveDegreeToZeroFiveItem(const FiveDegreeToZeroFiveItem& rhs) : nodes(rhs.nodes), id(rhs.id) {}

    template <
        class ElemT,
        class FunctionT,
        class T = decltype(std::declval<FunctionT>()(std::declval<ElemT>()))
    >
    const T map_and_sum(FunctionT fun) const {
        return iterate_and_sum<ElemT>(std::forward<FunctionT>(fun));
    }

    template <
        class ElemT,
        class FunctionT
    >
    const void map(FunctionT fun) const {
        iterate<ElemT>(std::forward<FunctionT>(fun));
    }
};

using GridItem = FiveDegreeToZeroFiveItem;

class ResolutionIterator : public iterator<random_access_iterator_tag, GridItem>{
    GridItem item;
    Step current_item = 0;
    const NodeVector vector;
    const LookupTable lu_table;
    const IDTable id_table;
    const size_type elementsInBigGrid;
 public:
    ResolutionIterator(NodeVector vector, LookupTable lu_table, IDTable id_table, size_type elementsInBigGrid)
        : item(GridItem(vector, lu_table, id_table, 0)),
          vector(vector), lu_table(lu_table), id_table(id_table), elementsInBigGrid(elementsInBigGrid) {}
    ResolutionIterator(NodeVector vector, LookupTable lu_table, IDTable id_table, size_type elementsInBigGrid, Step step) :
        item(GridItem(vector, lu_table, id_table, step)), vector(vector),
        lu_table(lu_table), id_table(id_table), elementsInBigGrid(elementsInBigGrid), current_item(step) {}
    ResolutionIterator(const ResolutionIterator &iterator) :
        item(iterator.item), vector(iterator.vector), lu_table(iterator.lu_table), id_table(iterator.id_table),
        elementsInBigGrid(iterator.elementsInBigGrid), current_item(iterator.current_item) {}
    ResolutionIterator& operator++() {
        current_item++;
        item = move(GridItem(vector, lu_table, id_table, current_item));
        return *this;
    }

    ResolutionIterator begin() const {return ResolutionIterator(this->vector, this->lu_table, this->id_table, this->elementsInBigGrid);}
    ResolutionIterator end() const {return ResolutionIterator(this->vector, this->lu_table, this->id_table, this->elementsInBigGrid, this->elementsInBigGrid);}

    //ResolutionIterator operator++(GridItem&) {ResolutionIterator tmp(*this); operator++(); return tmp;}
    bool operator==(const ResolutionIterator& rhs) {return item.id == rhs.item.id;}
    bool operator!=(const ResolutionIterator& rhs) {return item.id != rhs.item.id;}
    GridItem& operator*() {return item;}
    GridItem *get(size_type col) const {
        assert(col < elementsInBigGrid);
        return new GridItem(vector, lu_table, id_table, col);}
    GridItem operator[](size_type index) {
        assert(index < elementsInBigGrid);
        return GridItem(vector, lu_table, id_table, index);
    }
    size_type size() {return elementsInBigGrid;}
};

#endif //GROUNDWATER_RESOLUTIONITERATOR_HPP
