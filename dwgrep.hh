#ifndef _DWGREP_H_
#define _DWGREP_H_

#include <memory>
#include <vector>

#include <elfutils/libdw.h> // XXX

// A dwgrep_graph object represents a graph that we want to explore,
// and any associated caches.
class dwgrep_graph
{
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

public:
  typedef std::shared_ptr <dwgrep_graph> sptr;
  std::shared_ptr <Dwarf> dwarf;

  dwgrep_graph (std::shared_ptr <Dwarf> d);
  ~dwgrep_graph ();

  static Dwarf_Off const none_off = (Dwarf_Off) -1;
  Dwarf_Off find_parent (Dwarf_Die die);
  bool is_root (Dwarf_Die die);
};

class dwgrep_expr
{
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

public:
  class result;

  explicit dwgrep_expr (std::string const &str);
  ~dwgrep_expr ();

  result query (std::shared_ptr <dwgrep_graph> p);
  result query (dwgrep_graph p);
};

class dwgrep_expr::result
{
  friend class dwgrep_expr;
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

  explicit result (std::unique_ptr <pimpl> p);

public:
  ~result ();
  result (result &&that);
  result (result const &that) = delete;

  class iterator;
  iterator begin ();
  iterator end ();
};

class dwgrep_expr::result::iterator
{
  friend class dwgrep_expr::result;
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

  iterator (std::unique_ptr <pimpl> p);

public:
  iterator ();
  iterator (iterator const &other);
  ~iterator ();

  std::vector <int> operator* () const;

  iterator operator++ ();
  iterator operator++ (int);

  bool operator== (iterator that);
  bool operator!= (iterator that);
};

#endif /* _DWGREP_H_ */
