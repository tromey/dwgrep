#include "builtin.hh"
#include "op.hh"

struct
  : public builtin
{
  struct op_drop
    : public inner_op
  {
    using inner_op::inner_op;

    valfile::uptr
    next () override
    {
      if (auto vf = m_upstream->next ())
	{
	  vf->pop ();
	  return vf;
	}
      return nullptr;
    }

    std::string
    name () const override
    {
      return "drop";
    }
  };

  std::shared_ptr <op>
  build_exec (std::shared_ptr <op> upstream, dwgrep_graph::sptr q,
	      std::shared_ptr <scope> scope) const override
  {
    return std::make_shared <op_drop> (upstream);
  }

  char const *
  name () const override
  {
    return "drop";
  }
} builtin_drop_obj;


static struct
  : public builtin
{
  struct op_swap
    : public inner_op
  {
    using inner_op::inner_op;

    valfile::uptr
    next () override
    {
      if (auto vf = m_upstream->next ())
	{
	  auto a = vf->pop ();
	  auto b = vf->pop ();
	  vf->push (std::move (a));
	  vf->push (std::move (b));
	  return vf;
	}
      return nullptr;
    }

    std::string
    name () const override
    {
      return "swap";
    }
  };

  std::shared_ptr <op>
  build_exec (std::shared_ptr <op> upstream, dwgrep_graph::sptr q,
	      std::shared_ptr <scope> scope) const override
  {
    return std::make_shared <op_swap> (upstream);
  }

  char const *
  name () const override
  {
    return "swap";
  }
} builtin_swap_obj;

static struct
  : public builtin
{
  struct op_dup
    : public inner_op
  {
    using inner_op::inner_op;

    valfile::uptr
    next () override
    {
      if (auto vf = m_upstream->next ())
	{
	  vf->push (vf->top ().clone ());
	  return vf;
	}
      return nullptr;
    }

    std::string
    name () const override
    {
      return "dup";
    }
  };

  std::shared_ptr <op>
  build_exec (std::shared_ptr <op> upstream, dwgrep_graph::sptr q,
	      std::shared_ptr <scope> scope) const override
  {
    return std::make_shared <op_dup> (upstream);
  }

  char const *
  name () const override
  {
    return "dup";
  }
} builtin_dup_obj;

static struct
  : public builtin
{
  struct op_over
    : public inner_op
  {
    using inner_op::inner_op;

    valfile::uptr
    next () override
    {
      if (auto vf = m_upstream->next ())
	{
	  vf->push (vf->below ().clone ());
	  return vf;
	}
      return nullptr;
    }

    std::string
    name () const override
    {
      return "over";
    }
  };

  std::shared_ptr <op>
  build_exec (std::shared_ptr <op> upstream, dwgrep_graph::sptr q,
	      std::shared_ptr <scope> scope) const override
  {
    return std::make_shared <op_over> (upstream);
  }

  char const *
  name () const override
  {
    return "over";
  }
} builtin_over_obj;

static struct register_shf
{
  register_shf ()
  {
    add_builtin (builtin_drop_obj);
    add_builtin (builtin_swap_obj);
    add_builtin (builtin_dup_obj);
    add_builtin (builtin_over_obj);
  }
} register_shf;