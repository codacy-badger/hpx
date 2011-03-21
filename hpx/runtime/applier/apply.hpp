//  Copyright (c) 2007-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_APPLIER_APPLY_NOV_27_2008_0957AM)
#define HPX_APPLIER_APPLY_NOV_27_2008_0957AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/runtime/applier/apply_helper.hpp>
#include <hpx/runtime/actions/component_action.hpp>

namespace hpx { namespace applier 
{
    ///////////////////////////////////////////////////////////////////////////
    // zero parameter version of apply()
    // Invoked by a running PX-thread to apply an action to any resource

    /// \note A call to applier's apply function would look like:
    /// \code
    ///    appl_.apply<add_action>(gid, ...);
    /// \endcode

    // we know, it's remote
    template <typename Action>
    inline bool 
    apply_r_p(naming::address& addr, naming::id_type const& gid,
        threads::thread_priority priority)
    {
        // If remote, create a new parcel to be sent to the destination
        // Create a new parcel with the gid, action, and arguments
        parcelset::parcel p (gid.get_gid(), new Action(priority));
        if (components::component_invalid == addr.type_)
            addr.type_ = components::get_component_type<typename Action::component_type>();
        p.set_destination_addr(addr);   // avoid to resolve address again

        // Send the parcel through the parcel handler
        hpx::applier::get_applier().get_parcel_handler().put_parcel(p);
        return false;     // destination is remote
    }

    template <typename Action>
    inline bool 
    apply_r (naming::address& addr, naming::id_type const& gid)
    {
        return apply_r_p<Action>(addr, gid, threads::thread_priority_default);
    }

    // we  know, it's local and has to be directly executed
    template <typename Action>
    inline bool 
    apply_l_p(naming::address const& addr, threads::thread_priority priority)
    {
        BOOST_ASSERT(components::types_are_compatible(addr.type_, 
            components::get_component_type<typename Action::component_type>()));
        detail::apply_helper0<Action>::call(addr.address_, priority);
        return true;     // no parcel has been sent (dest is local)
    }

    template <typename Action>
    inline bool 
    apply_l (naming::address const& addr)
    {
        return apply_l_p<Action>(addr, threads::thread_priority_default);
    }

    template <typename Action>
    inline bool 
    apply_p (naming::id_type const& gid, threads::thread_priority priority)
    {
        // Determine whether the gid is local or remote
        naming::address addr;
        if (hpx::applier::get_applier().address_is_local(gid, addr)) 
            return apply_l_p<Action>(addr, priority);   // apply locally

        // apply remotely
        return apply_r_p<Action>(addr, gid, priority);
    }

    template <typename Action>
    inline bool apply (naming::id_type const& gid)
    {
        return apply_p<Action>(gid, threads::thread_priority_default);
    }

//     template <typename Action>
//     bool apply (naming::full_address& fa)
//     {
//         // Determine whether the gid is local or remote
//         if (hpx::applier::get_applier().address_is_local(fa)) 
//             return apply_l<Action>(fa.caddr());   // apply locally
// 
//         // apply remotely
//         return apply_r<Action>(fa.addr(), fa.cgid());
//     }

    /// \note A call to applier's apply function would look like:
    /// \code
    ///    appl_.apply<add_action>(cont, gid, ...);
    /// \endcode
    template <typename Action>
    inline bool 
    apply_r_p(naming::address& addr, actions::continuation* c, 
        naming::id_type const& gid, threads::thread_priority priority)
    {
        actions::continuation_type cont(c);

        // If remote, create a new parcel to be sent to the destination
        // Create a new parcel with the gid, action, and arguments
        parcelset::parcel p (gid.get_gid(), new Action(priority), cont);
        if (components::component_invalid == addr.type_)
            addr.type_ = components::get_component_type<typename Action::component_type>();
        p.set_destination_addr(addr);   // avoid to resolve address again

        // Send the parcel through the parcel handler
        hpx::applier::get_applier().get_parcel_handler().put_parcel(p);
        return false;     // destination is remote
    }

    template <typename Action>
    inline bool 
    apply_r (naming::address& addr, actions::continuation* c, 
        naming::id_type const& gid)
    {
        return apply_r_p<Action>(addr, c, gid, threads::thread_priority_default);
    }

    // we  know, it's local and has to be directly executed
    template <typename Action>
    inline bool apply_l_p(actions::continuation* c, 
        naming::address const& addr, threads::thread_priority priority)
    {
        BOOST_ASSERT(components::types_are_compatible(addr.type_, 
            components::get_component_type<typename Action::component_type>()));
        actions::continuation_type cont(c);
        detail::apply_helper0<Action>::call(cont, addr.address_, priority);
        return true;     // no parcel has been sent (dest is local)
    }

    template <typename Action>
    inline bool apply_l (actions::continuation* c, naming::address const& addr)
    {
        return apply_l_p<Action>(c, addr, threads::thread_priority_default);
    }

    template <typename Action>
    inline bool apply_p(actions::continuation* c, naming::id_type const& gid, 
        threads::thread_priority priority)
    {
        // Determine whether the gid is local or remote
        naming::address addr;
        if (hpx::applier::get_applier().address_is_local(gid, addr)) 
            return apply_l_p<Action>(c, addr, priority);

        // apply remotely
        return apply_r_p<Action>(addr, c, gid, priority);
    }

    template <typename Action>
    inline bool apply (actions::continuation* c, naming::id_type const& gid)
    {
        return apply_p<Action>(c, gid, threads::thread_priority_default);
    }

//     template <typename Action>
//     bool apply (actions::continuation* c, naming::full_address& fa)
//     {
//         // Determine whether the gid is local or remote
//         if (hpx::applier::get_applier().address_is_local(fa)) 
//             return apply_l<Action>(c, fa.caddr());
// 
//         // apply remotely
//         return apply_r<Action>(fa.addr(), c, fa.cgid());
//     }

    template <typename Action>
    inline bool 
    apply_c_p(naming::address& addr, naming::id_type const& contgid, 
        naming::id_type const& gid, threads::thread_priority priority)
    {
        return apply_r_p<Action>(addr, new actions::continuation(contgid), 
            gid, priority);
    }

    template <typename Action>
    inline bool 
    apply_c (naming::address& addr, naming::id_type const& contgid, 
        naming::id_type const& gid)
    {
        return apply_r<Action>(addr, new actions::continuation(contgid), gid);
    }

//     template <typename Action>
//     bool apply_c (naming::address& addr, naming::full_address const& contaddr, 
//         naming::id_type const& gid)
//     {
//         return apply_r<Action>(addr, new actions::continuation(contaddr), gid);
//     }

    template <typename Action>
    inline bool 
    apply_c_p(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority)
    {
        return apply_p<Action>(new actions::continuation(contgid), gid, priority);
    }

    template <typename Action>
    inline bool 
    apply_c (naming::id_type const& contgid, naming::id_type const& gid)
    {
        return apply<Action>(new actions::continuation(contgid), gid);
    }

//     template <typename Action>
//     bool apply_c (naming::full_address const& contaddr, naming::id_type const& gid)
//     {
//         return apply<Action>(new actions::continuation(contaddr), gid);
//     }

    ///////////////////////////////////////////////////////////////////////////
    // one parameter version
    template <typename Action, typename Arg0>
    inline bool 
    apply_r_p(naming::address& addr, naming::id_type const& gid, 
        threads::thread_priority priority, Arg0 const& arg0)
    {
        // If remote, create a new parcel to be sent to the destination
        // Create a new parcel with the gid, action, and arguments
        parcelset::parcel p (gid.get_gid(), new Action(priority, arg0));
        if (components::component_invalid == addr.type_)
            addr.type_ = components::get_component_type<typename Action::component_type>();
        p.set_destination_addr(addr);   // avoid to resolve address again

        // Send the parcel through the parcel handler
        hpx::applier::get_applier().get_parcel_handler().put_parcel(p);
        return false;     // destination is remote
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_r (naming::address& addr, naming::id_type const& gid, 
        Arg0 const& arg0)
    {
        return apply_r_p<Action>(addr, gid, threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_r_sync_p(naming::address& addr, naming::id_type const& gid, 
        threads::thread_priority priority, Arg0 const& arg0)
    {
        // If remote, create a new parcel to be sent to the destination
        // Create a new parcel with the gid, action, and arguments
        parcelset::parcel p (gid.get_gid(), new Action(priority, arg0));
        if (components::component_invalid == addr.type_)
            addr.type_ = components::get_component_type<typename Action::component_type>();
        p.set_destination_addr(addr);   // avoid to resolve address again

        // Send the parcel through the parcel handler
        hpx::applier::get_applier().get_parcel_handler().sync_put_parcel(p);
        return false;     // destination is remote
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_r_sync (naming::address& addr, naming::id_type const& gid, 
        Arg0 const& arg0)
    {
        return apply_r_sync_p<Action>(addr, gid, 
            threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_l_p(naming::address const& addr, threads::thread_priority priority, 
        Arg0 const& arg0)
    {
        BOOST_ASSERT(components::types_are_compatible(addr.type_, 
            components::get_component_type<typename Action::component_type>()));
        detail::apply_helper1<Action, Arg0>::call(addr.address_, priority, arg0);
        return true;     // no parcel has been sent (dest is local)
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_l (naming::address const& addr, Arg0 const& arg0)
    {
        return apply_l_p<Action>(addr, threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_p(naming::id_type const& gid, threads::thread_priority priority, 
        Arg0 const& arg0)
    {
        // Determine whether the gid is local or remote
        naming::address addr;
        if (hpx::applier::get_applier().address_is_local(gid, addr)) 
            return apply_l_p<Action>(addr, priority, arg0);   // apply locally

        // apply remotely
        return apply_r_p<Action>(addr, gid, priority, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply (naming::id_type const& gid, Arg0 const& arg0)
    {
        return apply_p<Action>(gid, threads::thread_priority_default, arg0);
    }

//     template <typename Action, typename Arg0>
//     bool apply (naming::full_address& fa, Arg0 const& arg0)
//     {
//         // Determine whether the gid is local or remote
//         if (hpx::applier::get_applier().address_is_local(fa)) 
//             return apply_l<Action>(fa.caddr(), arg0);   // apply locally
// 
//         // apply remotely
//         return apply_r<Action>(fa.addr(), fa.cgid(), arg0);
//     }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Action, typename Arg0>
    inline bool 
    apply_r_p(naming::address& addr, actions::continuation* c, 
        naming::id_type const& gid, threads::thread_priority priority,
        Arg0 const& arg0)
    {
        actions::continuation_type cont(c);

        // If remote, create a new parcel to be sent to the destination
        // Create a new parcel with the gid, action, and arguments
        parcelset::parcel p (gid.get_gid(), new Action(priority, arg0), cont);
        if (components::component_invalid == addr.type_)
            addr.type_ = components::get_component_type<typename Action::component_type>();
        p.set_destination_addr(addr);   // avoid to resolve address again

        // Send the parcel through the parcel handler
        hpx::applier::get_applier().get_parcel_handler().put_parcel(p);
        return false;     // destination is remote
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_r (naming::address& addr, actions::continuation* c, 
        naming::id_type const& gid, Arg0 const& arg0)
    {
        return apply_r_p<Action>(addr, c, gid, 
            threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_l_p(actions::continuation* c, naming::address const& addr, 
        threads::thread_priority priority, Arg0 const& arg0)
    {
        BOOST_ASSERT(components::types_are_compatible(addr.type_, 
            components::get_component_type<typename Action::component_type>()));
        actions::continuation_type cont(c);
        detail::apply_helper1<Action, Arg0>::call(cont, addr.address_, priority, arg0);
        return true;     // no parcel has been sent (dest is local)
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_l (actions::continuation* c, naming::address const& addr, 
        Arg0 const& arg0)
    {
        return apply_l_p<Action>(c, addr, threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_p(actions::continuation* c, naming::id_type const& gid, 
        threads::thread_priority priority, Arg0 const& arg0)
    {
        // Determine whether the gid is local or remote
        naming::address addr;
        if (hpx::applier::get_applier().address_is_local(gid, addr)) 
            return apply_l_p<Action>(c, addr, priority, arg0);    // apply locally

        // apply remotely
        return apply_r_p<Action>(addr, c, gid, priority, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply (actions::continuation* c, naming::id_type const& gid, 
        Arg0 const& arg0)
    {
        return apply_p<Action>(c, gid, threads::thread_priority_default, arg0);
    }

//     template <typename Action, typename Arg0>
//     bool apply (actions::continuation* c, naming::full_address& fa, 
//         Arg0 const& arg0)
//     {
//         // Determine whether the gid is local or remote
//         if (hpx::applier::get_applier().address_is_local(fa)) 
//             return apply_l<Action>(c, fa.caddr(), arg0);    // apply locally
// 
//         // apply remotely
//         return apply_r<Action>(fa.addr(), c, fa.cgid(), arg0);
//     }

    template <typename Action, typename Arg0>
    inline bool 
    apply_c_p(naming::address& addr, naming::id_type const& contgid, 
        naming::id_type const& gid, threads::thread_priority priority,
        Arg0 const& arg0)
    {
        return apply_r_p<Action>(addr, new actions::continuation(contgid), gid, 
            threads::thread_priority_default, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_c (naming::address& addr, naming::id_type const& contgid, 
        naming::id_type const& gid, Arg0 const& arg0)
    {
        return apply_r<Action>(addr, new actions::continuation(contgid), gid, arg0);
    }

//     template <typename Action, typename Arg0>
//     bool apply_c (naming::address& addr, naming::full_address const& contaddr, 
//         naming::id_type const& gid, Arg0 const& arg0)
//     {
//         return apply_r<Action>(addr, new actions::continuation(contaddr), gid, arg0);
//     }

    template <typename Action, typename Arg0>
    inline bool 
    apply_c_p(naming::id_type const& contgid, naming::id_type const& gid, 
        threads::thread_priority priority, Arg0 const& arg0)
    {
        return apply_p<Action>(new actions::continuation(contgid), gid, 
            priority, arg0);
    }

    template <typename Action, typename Arg0>
    inline bool 
    apply_c (naming::id_type const& contgid, naming::id_type const& gid, 
        Arg0 const& arg0)
    {
        return apply<Action>(new actions::continuation(contgid), gid, arg0);
    }

//     template <typename Action, typename Arg0>
//     bool apply_c (naming::full_address const& contaddr, naming::id_type const& gid, 
//         Arg0 const& arg0)
//     {
//         return apply<Action>(new actions::continuation(contaddr), gid, arg0);
//     }

    // bring in the rest of the apply<> overloads
    #include <hpx/runtime/applier/apply_implementations.hpp>

}}

#endif
