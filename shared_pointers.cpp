#include <assert.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <functional>


typedef long long ll;

template<typename T>
class ListIntf {
public:
  virtual T hd() = 0;
  virtual std::shared_ptr<ListIntf<T>> tl() = 0;
  virtual void rev_iter(std::function<void(T)> f) = 0;
};

template<typename T>
class EmptyList : public ListIntf<T> {
public:
  T hd() {
    assert(false);
    return T();
  }

  std::shared_ptr<ListIntf<T>> tl() {
    assert(false);
    return std::shared_ptr<ListIntf<T>>(NULL);
  }

  void rev_iter(std::function<void(T)> unused_f) {}

  EmptyList() {}
};


template<typename T>
class List : public ListIntf<T> {
public:
  T m_hd;
  std::shared_ptr<ListIntf<T>> m_tl;

  std::shared_ptr<ListIntf<T>> tl()
  {
    return m_tl;
  }

  T hd()
  {
    return m_hd;
  }

  void rev_iter(std::function<void(T)> f)
  {
    m_tl->rev_iter(f);
    f(m_hd);
  }

  List(T hd, std::shared_ptr<ListIntf<T>> tl) : m_hd(hd), m_tl(tl) {}
};


template<typename T>
using listptr = std::shared_ptr<ListIntf<T>>;


template<typename T>
listptr<T> append(const T elem, const listptr<T> l) {
  return std::make_shared<List<T>>(elem, l);
}

enum label_t {
  INLINED,
  ORIGINAL,
};

struct node_id_t {
  const int stamp;
  const listptr<std::shared_ptr<node_id_t>> parents;

  node_id_t(int stamp, listptr<std::shared_ptr<node_id_t>> parents) :
   stamp(stamp), parents(parents) {

  }

  node_id_t(int stamp) :
    stamp(stamp),
    parents(std::make_shared<EmptyList<std::shared_ptr<node_id_t>>>()) {

  }
};


struct tree_t {
  label_t label;
  std::shared_ptr<node_id_t> id;
  std::vector<std::shared_ptr<tree_t>> children;

  tree_t(
    label_t label,
    std::shared_ptr<node_id_t> id,
    std::vector<std::shared_ptr<tree_t>> children
  ) : label(label), id(id), children(children)
  {
  }
};

typedef std::shared_ptr<tree_t> treeptr;
typedef std::shared_ptr<node_id_t> idptr;

static idptr make_sym(int i)
{
  return std::make_shared<node_id_t>(i);
}


std::vector<treeptr> merge(
    const std::shared_ptr<tree_t> declaration,
    const std::shared_ptr<tree_t> application)
{
  std::vector<treeptr> children;

  for (const auto child : declaration->children) {
    auto new_id = std::make_shared<node_id_t>(
        child->id->stamp, append(application->id, child->id->parents)
    );
    auto new_child = std::make_shared<tree_t>(
        child->label, new_id, child->children
    );
    children.push_back(new_child);
  }

  return children;
}


treeptr construct_tree(std::istream & fin)
{
  std::map<int, std::vector<int>> cache;

  ll u, v;
  while (fin >> u >> v) {
    cache[u].push_back(v);
  }

  std::function<treeptr(int)> construct = [&](const int k){

    std::vector<int> children_stamps = cache[k];
    std::vector<treeptr> children;

    for (const int v: children_stamps) {
      children.push_back(construct(v));
    }

    return std::make_shared<tree_t>(ORIGINAL, make_sym(k), children);
  };


  return construct(0);
}

void print_id(std::ostream & out, idptr id)
{
  id->parents->rev_iter([&](idptr a){ print_id(out, a); });
  out << "/" << id->stamp;
}

void print_tree(std::ostream & out, treeptr tree, int depth = 0)
{
  for (int i = 0 ; i < depth ; i++) {
    out << " ";
  }
  print_id(out, tree->id);
  out << "\n";
  for (auto child : tree->children) {
    print_tree(out, child, depth + 1);
  }
}

std::pair<treeptr, treeptr>
find_earliest_leaf(treeptr tree)
{
  for (auto child : tree->children) {
    if(child->children.size() == 0) {
      return std::make_pair(tree, child);
    }
  }

  return find_earliest_leaf(tree->children.back());
}



int main(int argc, char *argv[])
{
  treeptr tree = construct_tree(std::cin);

  const auto p = find_earliest_leaf(tree);
  auto decl = p.first;
  auto callsite = p.second;

  std::vector<treeptr> garbage_nodes;

  int N = atoi(argv[1]);

  for (int i = 0; i < N ; i++) {
    auto b = merge(decl, callsite);
    garbage_nodes.insert(garbage_nodes.end(), b.begin(), b.end());

    for (auto c : b) {
      if (c->children.size() == 0) {
        callsite = c;
        break;
      }
    }
  }

  std::cout << "Number of children decl = " << decl->children.size() << std::endl;
  std::cout << garbage_nodes.size() << std::endl;
  
  return 0;
}
