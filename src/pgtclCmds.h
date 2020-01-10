/*-------------------------------------------------------------------------
 *
 * pgtclCmds.h
 *	  declarations for the C functions which implement pg_* tcl commands
 *
 * Portions Copyright (c) 1996-2004, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Id: pgtclCmds.h,v 1.5 2007/07/06 02:17:15 lbayuk Exp $
 *
 *-------------------------------------------------------------------------
 */

#ifndef PGTCLCMDS_H
#define PGTCLCMDS_H

#include <tcl.h>

#include "libpq-fe.h"

/* Hack to deal with Tcl 8.4 const-ification without losing compatibility */
#ifndef CONST84
#define CONST84
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define RES_HARD_MAX 128
#define RES_START 16

/*
 * Each Pg_ConnectionId has a list of Pg_TclNotifies structs, one for each
 * Tcl interpreter that has executed any pg_listens on the connection.
 * We need this arrangement to be able to clean up if an interpreter is
 * deleted while the connection remains open.  A free side benefit is that
 * multiple interpreters can be registered to listen for the same notify
 * name.  (All their callbacks will be called, but in an unspecified order.)
 *
 * We use the same approach for pg_on_connection_loss callbacks, but they
 * are not kept in a hashtable since there's no name associated.
 */

typedef struct Pg_TclNotifies_s
{
	struct Pg_TclNotifies_s *next;		/* list link */
	Tcl_Interp *interp;			/* This Tcl interpreter */

	/*
	 * NB: if interp == NULL, the interpreter is gone but we haven't yet
	 * got round to deleting the Pg_TclNotifies structure.
	 */
	Tcl_HashTable notify_hash;	/* Active pg_listen requests */

	char	   *conn_loss_cmd;	/* pg_on_connection_loss cmd, or NULL */
}	Pg_TclNotifies;

typedef struct Pg_ConnectionId_s
{
	char		id[32];
	PGconn	   *conn;
	int			res_max;		/* Max number of results allocated */
	int			res_hardmax;	/* Absolute max to allow */
	int			res_count;		/* Current count of active results */
	int			res_last;		/* Optimize where to start looking */
	int			res_copy;		/* Query result with active copy */
	int			res_copyStatus; /* Copying status */
	PGresult  **results;		/* The results */

	Pg_TclNotifies *notify_list;	/* head of list of notify info */
	int			notifier_running;		/* notify event source is live */
	Tcl_Channel notifier_channel;		/* Tcl_Channel on which notifier
										 * is listening */
	char	   *null_string;	/* String to return for NULL values */
	Tcl_Obj	   *notice_command;	/* Command to execute on getting a NOTICE */
	Tcl_Interp *interp;			/* Pointer to Tcl interpreter. This is
								   used by the notice handler callback. */
	char	   *copyBuf;		/* Buffer, if COPY TO record is too big */
	char	   *copyBufNext;	/* COPY TO overflow buffer remaining data */
	int			copyBufLeft;	/* COPY TO overflow buffer remaining bytes */
	Tcl_Obj    *callbackPtr;    /* Callback command prefix for async queries */
	Tcl_Interp *callbackInterp; /* Interp for the async query callback */
}	Pg_ConnectionId;

/* Values of res_copyStatus */
#define RES_COPY_NONE	0
#define RES_COPY_INPROGRESS 1
#define RES_COPY_FIN	2



/* **************************/
/* registered Tcl functions */
/* **************************/
extern int Pg_conndefaults(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_connect(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_disconnect(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_exec(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_execute(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_select(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_result(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_open(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_close(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_read(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_write(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_lseek(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_creat(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_tell(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_unlink(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_import(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_lo_export(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_listen(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_sendquery(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_sendquery_params(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_sendquery_prepared(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_getresult(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_isbusy(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_blocking(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_cancelrequest(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_on_connection_loss(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_escape_string(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_quote(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_escape_bytea(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_unescape_bytea(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_transaction_status(
  ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_parameter_status(
  ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_exec_prepared(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_exec_params(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_notice_handler(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int Pg_result_callback(
  ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

#endif   /* PGTCLCMDS_H */
