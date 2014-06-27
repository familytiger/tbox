/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        aico.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aico.h"
#include "aicp.h"
#include "impl/prefix.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aico_ref_t tb_aico_init_sock(tb_aicp_ref_t aicp, tb_socket_ref_t sock)
{
    // addo
    return tb_aicp_addo(aicp, (tb_handle_t)sock, TB_AICO_TYPE_SOCK);
}
tb_aico_ref_t tb_aico_init_file(tb_aicp_ref_t aicp, tb_file_ref_t file)
{
    // addo
    return tb_aicp_addo(aicp, (tb_handle_t)file, TB_AICO_TYPE_FILE);
}
tb_aico_ref_t tb_aico_init_task(tb_aicp_ref_t aicp, tb_bool_t bltimer)
{
    // addo, hack: handle != null? using higher precision timer for being compatible with sock/file impl task
    return tb_aicp_addo(aicp, (tb_handle_t)(tb_size_t)!bltimer, TB_AICO_TYPE_TASK);
}
tb_void_t tb_aico_kill(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return(impl && impl->aicp);

    // kilo
    tb_aicp_kilo((tb_aicp_ref_t)impl->aicp, aico);
}
tb_void_t tb_aico_exit(tb_aico_ref_t aico, tb_aico_exit_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return(impl && impl->aicp);

    // delo
    tb_aicp_delo((tb_aicp_ref_t)impl->aicp, aico, func, priv);
}
tb_aicp_ref_t tb_aico_aicp(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl, tb_null);

    // the impl aicp
    return impl->aicp;
}
tb_size_t tb_aico_type(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl, TB_AICO_TYPE_NONE);

    // the impl type
    return impl->type;
}
tb_handle_t tb_aico_pool(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl, tb_null);

    // the pool handle
    return impl->pool;
}
tb_socket_ref_t tb_aico_sock(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->type == TB_AICO_TYPE_SOCK, tb_null);

    // the socket handle
    return (tb_socket_ref_t)impl->handle;
}
tb_file_ref_t tb_aico_file(tb_aico_ref_t aico)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->type == TB_AICO_TYPE_FILE, tb_null);

    // the file handle
    return (tb_file_ref_t)impl->handle;
}
tb_long_t tb_aico_timeout(tb_aico_ref_t aico, tb_size_t type)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && type < tb_object_arrayn(impl->timeout), -1);

    // the impl timeout
    return tb_atomic_get((tb_atomic_t*)(impl->timeout + type));
}
tb_void_t tb_aico_timeout_set(tb_aico_ref_t aico, tb_size_t type, tb_long_t timeout)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return(impl && type < tb_object_arrayn(impl->timeout));

    // set the impl timeout
    tb_atomic_set((tb_atomic_t*)(impl->timeout + type), timeout);
}
tb_bool_t tb_aico_acpt_(tb_aico_ref_t aico, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_ACPT;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_conn_(tb_aico_ref_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_CONN;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.conn.port        = port;
    aice.u.conn.addr        = *addr;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recv_(tb_aico_ref_t aico, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_RECV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.recv.data        = data;
    aice.u.recv.size        = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_send_(tb_aico_ref_t aico, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SEND;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.send.data        = data;
    aice.u.send.size        = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecv_(tb_aico_ref_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_URECV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.urecv.data       = data;
    aice.u.urecv.size       = size;
    aice.u.urecv.port       = port;
    aice.u.urecv.addr       = *addr;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usend_(tb_aico_ref_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_USEND;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.usend.data       = data;
    aice.u.usend.size       = size;
    aice.u.usend.port       = port;
    aice.u.usend.addr       = *addr;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recvv_(tb_aico_ref_t aico, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_RECVV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.recvv.list       = list;
    aice.u.recvv.size       = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendv_(tb_aico_ref_t aico, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SENDV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.sendv.list       = list;
    aice.u.sendv.size       = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecvv_(tb_aico_ref_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_URECVV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.urecvv.list      = list;
    aice.u.urecvv.size      = size;
    aice.u.urecvv.port      = port;
    aice.u.urecvv.addr      = *addr;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usendv_(tb_aico_ref_t aico, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_USENDV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.usendv.list      = list;
    aice.u.usendv.size      = size;
    aice.u.usendv.port      = port;
    aice.u.usendv.addr      = *addr;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendf_(tb_aico_ref_t aico, tb_file_ref_t file, tb_hize_t seek, tb_hize_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && file, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SENDF;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.sendf.file       = file;
    aice.u.sendf.seek       = seek;
    aice.u.sendf.size       = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_read_(tb_aico_ref_t aico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_READ;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.read.seek        = seek;
    aice.u.read.data        = data;
    aice.u.read.size        = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writ_(tb_aico_ref_t aico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_WRIT;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.writ.seek        = seek;
    aice.u.writ.data        = data;
    aice.u.writ.size        = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_readv_(tb_aico_ref_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_READV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.readv.seek       = seek;
    aice.u.readv.list       = list;
    aice.u.readv.size       = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writv_(tb_aico_ref_t aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_WRITV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.writv.seek       = seek;
    aice.u.writv.list       = list;
    aice.u.writv.size       = size;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_fsync_(tb_aico_ref_t aico, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_FSYNC;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_acpt_after_(tb_aico_ref_t aico, tb_size_t delay, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_ACPT;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_conn_after_(tb_aico_ref_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_CONN;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.conn.port        = port;
    aice.u.conn.addr        = *addr;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_RECV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.recv.data        = data;
    aice.u.recv.size        = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_send_after_(tb_aico_ref_t aico, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SEND;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.send.data        = data;
    aice.u.send.size        = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_URECV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.urecv.data       = data;
    aice.u.urecv.size       = size;
    aice.u.urecv.port       = port;
    aice.u.urecv.addr       = *addr;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usend_after_(tb_aico_ref_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_USEND;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.usend.data       = data;
    aice.u.usend.size       = size;
    aice.u.usend.port       = port;
    aice.u.usend.addr       = *addr;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_recvv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_RECVV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.recvv.list       = list;
    aice.u.recvv.size       = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SENDV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.sendv.list       = list;
    aice.u.sendv.size       = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_urecvv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_URECVV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.urecvv.list      = list;
    aice.u.urecvv.size      = size;
    aice.u.urecvv.port      = port;
    aice.u.urecvv.addr      = *addr;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_usendv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_ipv4_t const* addr, tb_size_t port, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && addr && addr->u32 && port && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_USENDV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.usendv.list      = list;
    aice.u.usendv.size      = size;
    aice.u.usendv.port      = port;
    aice.u.usendv.addr      = *addr;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_sendf_after_(tb_aico_ref_t aico, tb_size_t delay, tb_file_ref_t file, tb_hize_t seek, tb_hize_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && file, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_SENDF;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.sendf.file       = file;
    aice.u.sendf.seek       = seek;
    aice.u.sendf.size       = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_read_after_(tb_aico_ref_t aico, tb_size_t delay, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_READ;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.read.seek        = seek;
    aice.u.read.data        = data;
    aice.u.read.size        = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writ_after_(tb_aico_ref_t aico, tb_size_t delay, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && data && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_WRIT;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.writ.seek        = seek;
    aice.u.writ.data        = data;
    aice.u.writ.size        = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_readv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_READV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.readv.seek       = seek;
    aice.u.readv.list       = list;
    aice.u.readv.size       = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_writv_after_(tb_aico_ref_t aico, tb_size_t delay, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp && list && size, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_WRITV;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.writv.seek       = seek;
    aice.u.writv.list       = list;
    aice.u.writv.size       = size;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}
tb_bool_t tb_aico_fsync_after_(tb_aico_ref_t aico, tb_size_t delay, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_FSYNC;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;

    // post
    return tb_aicp_post_after_((tb_aicp_ref_t)impl->aicp, delay, &aice __tb_debug_args__);
}

tb_bool_t tb_aico_task_run_(tb_aico_ref_t aico, tb_size_t delay, tb_aico_func_t func, tb_cpointer_t priv __tb_debug_decl__)
{
    // check
    tb_aico_impl_t* impl = (tb_aico_impl_t*)aico;
    tb_assert_and_check_return_val(impl && impl->aicp, tb_false);

    // init
    tb_aice_t               aice = {0};
    aice.code               = TB_AICE_CODE_RUNTASK;
    aice.state              = TB_STATE_PENDING;
    aice.func               = func;
    aice.priv               = priv;
    aice.aico               = aico;
    aice.u.runtask.when     = tb_cache_time_mclock() + delay;
    aice.u.runtask.delay    = delay;

    // post
    return tb_aicp_post_((tb_aicp_ref_t)impl->aicp, &aice __tb_debug_args__);
}

