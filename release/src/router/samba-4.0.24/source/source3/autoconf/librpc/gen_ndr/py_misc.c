
/* Python wrapper functions auto-generated by pidl */
#include <Python.h>
#include "includes.h"
#include <pytalloc.h>
#include "librpc/rpc/pyrpc.h"
#include "librpc/rpc/pyrpc_util.h"
#include "autoconf/librpc/gen_ndr/ndr_misc.h"
#include "autoconf/librpc/gen_ndr/ndr_misc_c.h"

staticforward PyTypeObject GUID_Type;
staticforward PyTypeObject ndr_syntax_id_Type;
staticforward PyTypeObject policy_handle_Type;
staticforward PyTypeObject KRB5_EDATA_NTSTATUS_Type;

void initmisc(void);static PyTypeObject *Object_Type;
#include "librpc/ndr/py_misc.c"


static PyObject *py_GUID_get_time_low(PyObject *obj, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(obj);
	PyObject *py_time_low;
	py_time_low = PyInt_FromLong(object->time_low);
	return py_time_low;
}

static int py_GUID_set_time_low(PyObject *py_obj, PyObject *value, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->time_low = PyInt_AsLong(value);
	return 0;
}

static PyObject *py_GUID_get_time_mid(PyObject *obj, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(obj);
	PyObject *py_time_mid;
	py_time_mid = PyInt_FromLong(object->time_mid);
	return py_time_mid;
}

static int py_GUID_set_time_mid(PyObject *py_obj, PyObject *value, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->time_mid = PyInt_AsLong(value);
	return 0;
}

static PyObject *py_GUID_get_time_hi_and_version(PyObject *obj, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(obj);
	PyObject *py_time_hi_and_version;
	py_time_hi_and_version = PyInt_FromLong(object->time_hi_and_version);
	return py_time_hi_and_version;
}

static int py_GUID_set_time_hi_and_version(PyObject *py_obj, PyObject *value, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->time_hi_and_version = PyInt_AsLong(value);
	return 0;
}

static PyObject *py_GUID_get_clock_seq(PyObject *obj, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(obj);
	PyObject *py_clock_seq;
	py_clock_seq = PyList_New(2);
	if (py_clock_seq == NULL) {
		return NULL;
	}
	{
		int clock_seq_cntr_0;
		for (clock_seq_cntr_0 = 0; clock_seq_cntr_0 < 2; clock_seq_cntr_0++) {
			PyObject *py_clock_seq_0;
			py_clock_seq_0 = PyInt_FromLong(object->clock_seq[clock_seq_cntr_0]);
			PyList_SetItem(py_clock_seq, clock_seq_cntr_0, py_clock_seq_0);
		}
	}
	return py_clock_seq;
}

static int py_GUID_set_clock_seq(PyObject *py_obj, PyObject *value, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyList_Type, value, return -1;);
	{
		int clock_seq_cntr_0;
		for (clock_seq_cntr_0 = 0; clock_seq_cntr_0 < PyList_GET_SIZE(value); clock_seq_cntr_0++) {
			PY_CHECK_TYPE(&PyInt_Type, PyList_GET_ITEM(value, clock_seq_cntr_0), return -1;);
			object->clock_seq[clock_seq_cntr_0] = PyInt_AsLong(PyList_GET_ITEM(value, clock_seq_cntr_0));
		}
	}
	return 0;
}

static PyObject *py_GUID_get_node(PyObject *obj, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(obj);
	PyObject *py_node;
	py_node = PyList_New(6);
	if (py_node == NULL) {
		return NULL;
	}
	{
		int node_cntr_0;
		for (node_cntr_0 = 0; node_cntr_0 < 6; node_cntr_0++) {
			PyObject *py_node_0;
			py_node_0 = PyInt_FromLong(object->node[node_cntr_0]);
			PyList_SetItem(py_node, node_cntr_0, py_node_0);
		}
	}
	return py_node;
}

static int py_GUID_set_node(PyObject *py_obj, PyObject *value, void *closure)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyList_Type, value, return -1;);
	{
		int node_cntr_0;
		for (node_cntr_0 = 0; node_cntr_0 < PyList_GET_SIZE(value); node_cntr_0++) {
			PY_CHECK_TYPE(&PyInt_Type, PyList_GET_ITEM(value, node_cntr_0), return -1;);
			object->node[node_cntr_0] = PyInt_AsLong(PyList_GET_ITEM(value, node_cntr_0));
		}
	}
	return 0;
}

static PyGetSetDef py_GUID_getsetters[] = {
	{ discard_const_p(char, "time_low"), py_GUID_get_time_low, py_GUID_set_time_low },
	{ discard_const_p(char, "time_mid"), py_GUID_get_time_mid, py_GUID_set_time_mid },
	{ discard_const_p(char, "time_hi_and_version"), py_GUID_get_time_hi_and_version, py_GUID_set_time_hi_and_version },
	{ discard_const_p(char, "clock_seq"), py_GUID_get_clock_seq, py_GUID_set_clock_seq },
	{ discard_const_p(char, "node"), py_GUID_get_node, py_GUID_set_node },
	{ NULL }
};

static PyObject *py_GUID_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	return pytalloc_new(struct GUID, type);
}

static PyObject *py_GUID_ndr_pack(PyObject *py_obj)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	enum ndr_err_code err;
	err = ndr_push_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_push_flags_fn_t)ndr_push_GUID);
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	return PyString_FromStringAndSize((char *)blob.data, blob.length);
}

static PyObject *py_GUID_ndr_unpack(PyObject *py_obj, PyObject *args, PyObject *kwargs)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	int blob_length = 0;
	enum ndr_err_code err;
	const char * const kwnames[] = { "data_blob", "allow_remaining", NULL };
	PyObject *allow_remaining_obj = NULL;
	bool allow_remaining = false;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O:__ndr_unpack__",
		discard_const_p(char *, kwnames),
		&blob.data, &blob_length,
		&allow_remaining_obj)) {
		return NULL;
	}
	blob.length = blob_length;

	if (allow_remaining_obj && PyObject_IsTrue(allow_remaining_obj)) {
		allow_remaining = true;
	}

	if (allow_remaining) {
		err = ndr_pull_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_GUID);
	} else {
		err = ndr_pull_struct_blob_all(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_GUID);
	}
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *py_GUID_ndr_print(PyObject *py_obj)
{
	struct GUID *object = (struct GUID *)pytalloc_get_ptr(py_obj);
	PyObject *ret;
	char *retstr;

	retstr = ndr_print_struct_string(pytalloc_get_mem_ctx(py_obj), (ndr_print_fn_t)ndr_print_GUID, "GUID", object);
	ret = PyString_FromString(retstr);
	talloc_free(retstr);

	return ret;
}

static PyMethodDef py_GUID_methods[] = {
	{ "__ndr_pack__", (PyCFunction)py_GUID_ndr_pack, METH_NOARGS, "S.ndr_pack(object) -> blob\nNDR pack" },
	{ "__ndr_unpack__", (PyCFunction)py_GUID_ndr_unpack, METH_VARARGS|METH_KEYWORDS, "S.ndr_unpack(class, blob, allow_remaining=False) -> None\nNDR unpack" },
	{ "__ndr_print__", (PyCFunction)py_GUID_ndr_print, METH_VARARGS, "S.ndr_print(object) -> None\nNDR print" },
	{ NULL, NULL, 0, NULL }
};


static PyTypeObject GUID_Type = {
	PyObject_HEAD_INIT(NULL) 0,
	.tp_name = "misc.GUID",
	.tp_getset = py_GUID_getsetters,
	.tp_methods = py_GUID_methods,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_basicsize = sizeof(pytalloc_Object),
	.tp_new = py_GUID_new,
};


static PyObject *py_ndr_syntax_id_get_uuid(PyObject *obj, void *closure)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(obj);
	PyObject *py_uuid;
	py_uuid = pytalloc_reference_ex(&GUID_Type, pytalloc_get_mem_ctx(obj), &object->uuid);
	return py_uuid;
}

static int py_ndr_syntax_id_set_uuid(PyObject *py_obj, PyObject *value, void *closure)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&GUID_Type, value, return -1;);
	if (talloc_reference(pytalloc_get_mem_ctx(py_obj), pytalloc_get_mem_ctx(value)) == NULL) {
		PyErr_NoMemory();
		return -1;
	}
	object->uuid = *(struct GUID *)pytalloc_get_ptr(value);
	return 0;
}

static PyObject *py_ndr_syntax_id_get_if_version(PyObject *obj, void *closure)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(obj);
	PyObject *py_if_version;
	py_if_version = PyInt_FromLong(object->if_version);
	return py_if_version;
}

static int py_ndr_syntax_id_set_if_version(PyObject *py_obj, PyObject *value, void *closure)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->if_version = PyInt_AsLong(value);
	return 0;
}

static PyGetSetDef py_ndr_syntax_id_getsetters[] = {
	{ discard_const_p(char, "uuid"), py_ndr_syntax_id_get_uuid, py_ndr_syntax_id_set_uuid },
	{ discard_const_p(char, "if_version"), py_ndr_syntax_id_get_if_version, py_ndr_syntax_id_set_if_version },
	{ NULL }
};

static PyObject *py_ndr_syntax_id_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	return pytalloc_new(struct ndr_syntax_id, type);
}

static PyObject *py_ndr_syntax_id_ndr_pack(PyObject *py_obj)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	enum ndr_err_code err;
	err = ndr_push_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_push_flags_fn_t)ndr_push_ndr_syntax_id);
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	return PyString_FromStringAndSize((char *)blob.data, blob.length);
}

static PyObject *py_ndr_syntax_id_ndr_unpack(PyObject *py_obj, PyObject *args, PyObject *kwargs)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	int blob_length = 0;
	enum ndr_err_code err;
	const char * const kwnames[] = { "data_blob", "allow_remaining", NULL };
	PyObject *allow_remaining_obj = NULL;
	bool allow_remaining = false;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O:__ndr_unpack__",
		discard_const_p(char *, kwnames),
		&blob.data, &blob_length,
		&allow_remaining_obj)) {
		return NULL;
	}
	blob.length = blob_length;

	if (allow_remaining_obj && PyObject_IsTrue(allow_remaining_obj)) {
		allow_remaining = true;
	}

	if (allow_remaining) {
		err = ndr_pull_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_ndr_syntax_id);
	} else {
		err = ndr_pull_struct_blob_all(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_ndr_syntax_id);
	}
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *py_ndr_syntax_id_ndr_print(PyObject *py_obj)
{
	struct ndr_syntax_id *object = (struct ndr_syntax_id *)pytalloc_get_ptr(py_obj);
	PyObject *ret;
	char *retstr;

	retstr = ndr_print_struct_string(pytalloc_get_mem_ctx(py_obj), (ndr_print_fn_t)ndr_print_ndr_syntax_id, "ndr_syntax_id", object);
	ret = PyString_FromString(retstr);
	talloc_free(retstr);

	return ret;
}

static PyMethodDef py_ndr_syntax_id_methods[] = {
	{ "__ndr_pack__", (PyCFunction)py_ndr_syntax_id_ndr_pack, METH_NOARGS, "S.ndr_pack(object) -> blob\nNDR pack" },
	{ "__ndr_unpack__", (PyCFunction)py_ndr_syntax_id_ndr_unpack, METH_VARARGS|METH_KEYWORDS, "S.ndr_unpack(class, blob, allow_remaining=False) -> None\nNDR unpack" },
	{ "__ndr_print__", (PyCFunction)py_ndr_syntax_id_ndr_print, METH_VARARGS, "S.ndr_print(object) -> None\nNDR print" },
	{ NULL, NULL, 0, NULL }
};


static PyTypeObject ndr_syntax_id_Type = {
	PyObject_HEAD_INIT(NULL) 0,
	.tp_name = "misc.ndr_syntax_id",
	.tp_getset = py_ndr_syntax_id_getsetters,
	.tp_methods = py_ndr_syntax_id_methods,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_basicsize = sizeof(pytalloc_Object),
	.tp_new = py_ndr_syntax_id_new,
};


static PyObject *py_policy_handle_get_handle_type(PyObject *obj, void *closure)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(obj);
	PyObject *py_handle_type;
	py_handle_type = PyInt_FromLong(object->handle_type);
	return py_handle_type;
}

static int py_policy_handle_set_handle_type(PyObject *py_obj, PyObject *value, void *closure)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->handle_type = PyInt_AsLong(value);
	return 0;
}

static PyObject *py_policy_handle_get_uuid(PyObject *obj, void *closure)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(obj);
	PyObject *py_uuid;
	py_uuid = pytalloc_reference_ex(&GUID_Type, pytalloc_get_mem_ctx(obj), &object->uuid);
	return py_uuid;
}

static int py_policy_handle_set_uuid(PyObject *py_obj, PyObject *value, void *closure)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&GUID_Type, value, return -1;);
	if (talloc_reference(pytalloc_get_mem_ctx(py_obj), pytalloc_get_mem_ctx(value)) == NULL) {
		PyErr_NoMemory();
		return -1;
	}
	object->uuid = *(struct GUID *)pytalloc_get_ptr(value);
	return 0;
}

static PyGetSetDef py_policy_handle_getsetters[] = {
	{ discard_const_p(char, "handle_type"), py_policy_handle_get_handle_type, py_policy_handle_set_handle_type },
	{ discard_const_p(char, "uuid"), py_policy_handle_get_uuid, py_policy_handle_set_uuid },
	{ NULL }
};

static PyObject *py_policy_handle_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	return pytalloc_new(struct policy_handle, type);
}

static PyObject *py_policy_handle_ndr_pack(PyObject *py_obj)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	enum ndr_err_code err;
	err = ndr_push_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_push_flags_fn_t)ndr_push_policy_handle);
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	return PyString_FromStringAndSize((char *)blob.data, blob.length);
}

static PyObject *py_policy_handle_ndr_unpack(PyObject *py_obj, PyObject *args, PyObject *kwargs)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	int blob_length = 0;
	enum ndr_err_code err;
	const char * const kwnames[] = { "data_blob", "allow_remaining", NULL };
	PyObject *allow_remaining_obj = NULL;
	bool allow_remaining = false;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O:__ndr_unpack__",
		discard_const_p(char *, kwnames),
		&blob.data, &blob_length,
		&allow_remaining_obj)) {
		return NULL;
	}
	blob.length = blob_length;

	if (allow_remaining_obj && PyObject_IsTrue(allow_remaining_obj)) {
		allow_remaining = true;
	}

	if (allow_remaining) {
		err = ndr_pull_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_policy_handle);
	} else {
		err = ndr_pull_struct_blob_all(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_policy_handle);
	}
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *py_policy_handle_ndr_print(PyObject *py_obj)
{
	struct policy_handle *object = (struct policy_handle *)pytalloc_get_ptr(py_obj);
	PyObject *ret;
	char *retstr;

	retstr = ndr_print_struct_string(pytalloc_get_mem_ctx(py_obj), (ndr_print_fn_t)ndr_print_policy_handle, "policy_handle", object);
	ret = PyString_FromString(retstr);
	talloc_free(retstr);

	return ret;
}

static PyMethodDef py_policy_handle_methods[] = {
	{ "__ndr_pack__", (PyCFunction)py_policy_handle_ndr_pack, METH_NOARGS, "S.ndr_pack(object) -> blob\nNDR pack" },
	{ "__ndr_unpack__", (PyCFunction)py_policy_handle_ndr_unpack, METH_VARARGS|METH_KEYWORDS, "S.ndr_unpack(class, blob, allow_remaining=False) -> None\nNDR unpack" },
	{ "__ndr_print__", (PyCFunction)py_policy_handle_ndr_print, METH_VARARGS, "S.ndr_print(object) -> None\nNDR print" },
	{ NULL, NULL, 0, NULL }
};


static PyTypeObject policy_handle_Type = {
	PyObject_HEAD_INIT(NULL) 0,
	.tp_name = "misc.policy_handle",
	.tp_getset = py_policy_handle_getsetters,
	.tp_methods = py_policy_handle_methods,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_basicsize = sizeof(pytalloc_Object),
	.tp_new = py_policy_handle_new,
};


static PyObject *py_KRB5_EDATA_NTSTATUS_get_ntstatus(PyObject *obj, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(obj);
	PyObject *py_ntstatus;
	py_ntstatus = PyErr_FromNTSTATUS(object->ntstatus);
	return py_ntstatus;
}

static int py_KRB5_EDATA_NTSTATUS_set_ntstatus(PyObject *py_obj, PyObject *value, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	object->ntstatus = NT_STATUS(PyInt_AsLong(value));
	return 0;
}

static PyObject *py_KRB5_EDATA_NTSTATUS_get_unknown1(PyObject *obj, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(obj);
	PyObject *py_unknown1;
	py_unknown1 = PyInt_FromLong(object->unknown1);
	return py_unknown1;
}

static int py_KRB5_EDATA_NTSTATUS_set_unknown1(PyObject *py_obj, PyObject *value, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->unknown1 = PyInt_AsLong(value);
	return 0;
}

static PyObject *py_KRB5_EDATA_NTSTATUS_get_unknown2(PyObject *obj, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(obj);
	PyObject *py_unknown2;
	py_unknown2 = PyInt_FromLong(object->unknown2);
	return py_unknown2;
}

static int py_KRB5_EDATA_NTSTATUS_set_unknown2(PyObject *py_obj, PyObject *value, void *closure)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	PY_CHECK_TYPE(&PyInt_Type, value, return -1;);
	object->unknown2 = PyInt_AsLong(value);
	return 0;
}

static PyGetSetDef py_KRB5_EDATA_NTSTATUS_getsetters[] = {
	{ discard_const_p(char, "ntstatus"), py_KRB5_EDATA_NTSTATUS_get_ntstatus, py_KRB5_EDATA_NTSTATUS_set_ntstatus },
	{ discard_const_p(char, "unknown1"), py_KRB5_EDATA_NTSTATUS_get_unknown1, py_KRB5_EDATA_NTSTATUS_set_unknown1 },
	{ discard_const_p(char, "unknown2"), py_KRB5_EDATA_NTSTATUS_get_unknown2, py_KRB5_EDATA_NTSTATUS_set_unknown2 },
	{ NULL }
};

static PyObject *py_KRB5_EDATA_NTSTATUS_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	return pytalloc_new(struct KRB5_EDATA_NTSTATUS, type);
}

static PyObject *py_KRB5_EDATA_NTSTATUS_ndr_pack(PyObject *py_obj)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	enum ndr_err_code err;
	err = ndr_push_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_push_flags_fn_t)ndr_push_KRB5_EDATA_NTSTATUS);
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	return PyString_FromStringAndSize((char *)blob.data, blob.length);
}

static PyObject *py_KRB5_EDATA_NTSTATUS_ndr_unpack(PyObject *py_obj, PyObject *args, PyObject *kwargs)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	DATA_BLOB blob;
	int blob_length = 0;
	enum ndr_err_code err;
	const char * const kwnames[] = { "data_blob", "allow_remaining", NULL };
	PyObject *allow_remaining_obj = NULL;
	bool allow_remaining = false;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O:__ndr_unpack__",
		discard_const_p(char *, kwnames),
		&blob.data, &blob_length,
		&allow_remaining_obj)) {
		return NULL;
	}
	blob.length = blob_length;

	if (allow_remaining_obj && PyObject_IsTrue(allow_remaining_obj)) {
		allow_remaining = true;
	}

	if (allow_remaining) {
		err = ndr_pull_struct_blob(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_KRB5_EDATA_NTSTATUS);
	} else {
		err = ndr_pull_struct_blob_all(&blob, pytalloc_get_mem_ctx(py_obj), object, (ndr_pull_flags_fn_t)ndr_pull_KRB5_EDATA_NTSTATUS);
	}
	if (err != NDR_ERR_SUCCESS) {
		PyErr_SetNdrError(err);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *py_KRB5_EDATA_NTSTATUS_ndr_print(PyObject *py_obj)
{
	struct KRB5_EDATA_NTSTATUS *object = (struct KRB5_EDATA_NTSTATUS *)pytalloc_get_ptr(py_obj);
	PyObject *ret;
	char *retstr;

	retstr = ndr_print_struct_string(pytalloc_get_mem_ctx(py_obj), (ndr_print_fn_t)ndr_print_KRB5_EDATA_NTSTATUS, "KRB5_EDATA_NTSTATUS", object);
	ret = PyString_FromString(retstr);
	talloc_free(retstr);

	return ret;
}

static PyMethodDef py_KRB5_EDATA_NTSTATUS_methods[] = {
	{ "__ndr_pack__", (PyCFunction)py_KRB5_EDATA_NTSTATUS_ndr_pack, METH_NOARGS, "S.ndr_pack(object) -> blob\nNDR pack" },
	{ "__ndr_unpack__", (PyCFunction)py_KRB5_EDATA_NTSTATUS_ndr_unpack, METH_VARARGS|METH_KEYWORDS, "S.ndr_unpack(class, blob, allow_remaining=False) -> None\nNDR unpack" },
	{ "__ndr_print__", (PyCFunction)py_KRB5_EDATA_NTSTATUS_ndr_print, METH_VARARGS, "S.ndr_print(object) -> None\nNDR print" },
	{ NULL, NULL, 0, NULL }
};


static PyTypeObject KRB5_EDATA_NTSTATUS_Type = {
	PyObject_HEAD_INIT(NULL) 0,
	.tp_name = "misc.KRB5_EDATA_NTSTATUS",
	.tp_getset = py_KRB5_EDATA_NTSTATUS_getsetters,
	.tp_methods = py_KRB5_EDATA_NTSTATUS_methods,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_basicsize = sizeof(pytalloc_Object),
	.tp_new = py_KRB5_EDATA_NTSTATUS_new,
};

PyObject *py_import_winreg_Data(TALLOC_CTX *mem_ctx, int level, union winreg_Data *in)
{
	PyObject *ret;

	switch (level) {
		case REG_NONE:
			ret = Py_None;
			Py_INCREF(ret);
			return ret;

		case REG_SZ:
			ret = PyString_FromStringOrNULL(in->string);
			return ret;

		case REG_EXPAND_SZ:
			ret = PyString_FromStringOrNULL(in->string);
			return ret;

		case REG_BINARY:
			ret = PyString_FromStringAndSize((char *)(in->binary).data, (in->binary).length);
			return ret;

		case REG_DWORD:
			ret = PyInt_FromLong(in->value);
			return ret;

		case REG_DWORD_BIG_ENDIAN:
			ret = PyInt_FromLong(in->value);
			return ret;

		case REG_MULTI_SZ:
			ret = pytalloc_CObject_FromTallocPtr(in->string_array);
			return ret;

		default:
			ret = PyString_FromStringAndSize((char *)(in->data).data, (in->data).length);
			return ret;

	}
	PyErr_SetString(PyExc_TypeError, "unknown union level");
	return NULL;
}

union winreg_Data *py_export_winreg_Data(TALLOC_CTX *mem_ctx, int level, PyObject *in)
{
	union winreg_Data *ret = talloc_zero(mem_ctx, union winreg_Data);
	switch (level) {
		case REG_NONE:
			break;

		case REG_SZ:
			ret->string = talloc_strdup(mem_ctx, PyString_AS_STRING(in));
			break;

		case REG_EXPAND_SZ:
			ret->string = talloc_strdup(mem_ctx, PyString_AS_STRING(in));
			break;

		case REG_BINARY:
			ret->binary = data_blob_talloc(mem_ctx, PyString_AS_STRING(in), PyString_GET_SIZE(in));
			break;

		case REG_DWORD:
			PY_CHECK_TYPE(&PyInt_Type, in, talloc_free(ret); return NULL;);
			ret->value = PyInt_AsLong(in);
			break;

		case REG_DWORD_BIG_ENDIAN:
			PY_CHECK_TYPE(&PyInt_Type, in, talloc_free(ret); return NULL;);
			ret->value = PyInt_AsLong(in);
			break;

		case REG_MULTI_SZ:
			ret->string_array = PyCObject_AsVoidPtr(in);
			break;

		default:
			ret->data = data_blob_talloc(mem_ctx, PyString_AS_STRING(in), PyString_GET_SIZE(in));
			break;

	}

	return ret;
}

static PyMethodDef misc_methods[] = {
	{ NULL, NULL, 0, NULL }
};

void initmisc(void)
{
	PyObject *m;
	PyObject *dep_talloc;

	dep_talloc = PyImport_ImportModule("talloc");
	if (dep_talloc == NULL)
		return;

	Object_Type = (PyTypeObject *)PyObject_GetAttrString(dep_talloc, "Object");
	if (Object_Type == NULL)
		return;

	GUID_Type.tp_base = Object_Type;

	ndr_syntax_id_Type.tp_base = Object_Type;

	policy_handle_Type.tp_base = Object_Type;

	KRB5_EDATA_NTSTATUS_Type.tp_base = Object_Type;

	if (PyType_Ready(&GUID_Type) < 0)
		return;
	if (PyType_Ready(&ndr_syntax_id_Type) < 0)
		return;
	if (PyType_Ready(&policy_handle_Type) < 0)
		return;
	if (PyType_Ready(&KRB5_EDATA_NTSTATUS_Type) < 0)
		return;
#ifdef PY_GUID_PATCH
	PY_GUID_PATCH(&GUID_Type);
#endif
#ifdef PY_NDR_SYNTAX_ID_PATCH
	PY_NDR_SYNTAX_ID_PATCH(&ndr_syntax_id_Type);
#endif
#ifdef PY_POLICY_HANDLE_PATCH
	PY_POLICY_HANDLE_PATCH(&policy_handle_Type);
#endif
#ifdef PY_KRB5_EDATA_NTSTATUS_PATCH
	PY_KRB5_EDATA_NTSTATUS_PATCH(&KRB5_EDATA_NTSTATUS_Type);
#endif

	m = Py_InitModule3("misc", misc_methods, "misc DCE/RPC");
	if (m == NULL)
		return;

	PyModule_AddObject(m, "SEC_CHAN_RODC", PyInt_FromLong(SEC_CHAN_RODC));
	PyModule_AddObject(m, "SV_TYPE_SQLSERVER", PyInt_FromLong(SV_TYPE_SQLSERVER));
	PyModule_AddObject(m, "SAM_DATABASE_PRIVS", PyInt_FromLong(SAM_DATABASE_PRIVS));
	PyModule_AddObject(m, "SV_TYPE_PRINTQ_SERVER", PyInt_FromLong(SV_TYPE_PRINTQ_SERVER));
	PyModule_AddObject(m, "SV_TYPE_NOVELL", PyInt_FromLong(SV_TYPE_NOVELL));
	PyModule_AddObject(m, "SEC_CHAN_DOMAIN", PyInt_FromLong(SEC_CHAN_DOMAIN));
	PyModule_AddObject(m, "REG_BINARY", PyInt_FromLong(REG_BINARY));
	PyModule_AddObject(m, "SV_TYPE_DOMAIN_MEMBER", PyInt_FromLong(SV_TYPE_DOMAIN_MEMBER));
	PyModule_AddObject(m, "SV_TYPE_DFS_SERVER", PyInt_FromLong(SV_TYPE_DFS_SERVER));
	PyModule_AddObject(m, "SV_TYPE_BACKUP_BROWSER", PyInt_FromLong(SV_TYPE_BACKUP_BROWSER));
	PyModule_AddObject(m, "REG_FULL_RESOURCE_DESCRIPTOR", PyInt_FromLong(REG_FULL_RESOURCE_DESCRIPTOR));
	PyModule_AddObject(m, "SV_TYPE_NT", PyInt_FromLong(SV_TYPE_NT));
	PyModule_AddObject(m, "REG_QWORD", PyInt_FromLong(REG_QWORD));
	PyModule_AddObject(m, "SV_TYPE_SERVER_OSF", PyInt_FromLong(SV_TYPE_SERVER_OSF));
	PyModule_AddObject(m, "SV_TYPE_SERVER_VMS", PyInt_FromLong(SV_TYPE_SERVER_VMS));
	PyModule_AddObject(m, "SV_TYPE_DOMAIN_CTRL", PyInt_FromLong(SV_TYPE_DOMAIN_CTRL));
	PyModule_AddObject(m, "SV_TYPE_WIN95_PLUS", PyInt_FromLong(SV_TYPE_WIN95_PLUS));
	PyModule_AddObject(m, "SAM_DATABASE_BUILTIN", PyInt_FromLong(SAM_DATABASE_BUILTIN));
	PyModule_AddObject(m, "SV_TYPE_SERVER", PyInt_FromLong(SV_TYPE_SERVER));
	PyModule_AddObject(m, "SV_TYPE_WORKSTATION", PyInt_FromLong(SV_TYPE_WORKSTATION));
	PyModule_AddObject(m, "SEC_CHAN_DNS_DOMAIN", PyInt_FromLong(SEC_CHAN_DNS_DOMAIN));
	PyModule_AddObject(m, "SV_TYPE_MASTER_BROWSER", PyInt_FromLong(SV_TYPE_MASTER_BROWSER));
	PyModule_AddObject(m, "SV_TYPE_AFP", PyInt_FromLong(SV_TYPE_AFP));
	PyModule_AddObject(m, "SEC_CHAN_NULL", PyInt_FromLong(SEC_CHAN_NULL));
	PyModule_AddObject(m, "SEC_CHAN_BDC", PyInt_FromLong(SEC_CHAN_BDC));
	PyModule_AddObject(m, "SV_TYPE_WFW", PyInt_FromLong(SV_TYPE_WFW));
	PyModule_AddObject(m, "REG_LINK", PyInt_FromLong(REG_LINK));
	PyModule_AddObject(m, "SV_TYPE_POTENTIAL_BROWSER", PyInt_FromLong(SV_TYPE_POTENTIAL_BROWSER));
	PyModule_AddObject(m, "SV_TYPE_DOMAIN_BAKCTRL", PyInt_FromLong(SV_TYPE_DOMAIN_BAKCTRL));
	PyModule_AddObject(m, "REG_SZ", PyInt_FromLong(REG_SZ));
	PyModule_AddObject(m, "SV_TYPE_DOMAIN_MASTER", PyInt_FromLong(SV_TYPE_DOMAIN_MASTER));
	PyModule_AddObject(m, "REG_DWORD_BIG_ENDIAN", PyInt_FromLong(REG_DWORD_BIG_ENDIAN));
	PyModule_AddObject(m, "REG_EXPAND_SZ", PyInt_FromLong(REG_EXPAND_SZ));
	PyModule_AddObject(m, "SEC_CHAN_WKSTA", PyInt_FromLong(SEC_CHAN_WKSTA));
	PyModule_AddObject(m, "SV_TYPE_SERVER_NT", PyInt_FromLong(SV_TYPE_SERVER_NT));
	PyModule_AddObject(m, "REG_MULTI_SZ", PyInt_FromLong(REG_MULTI_SZ));
	PyModule_AddObject(m, "SEC_CHAN_LANMAN", PyInt_FromLong(SEC_CHAN_LANMAN));
	PyModule_AddObject(m, "SAM_DATABASE_DOMAIN", PyInt_FromLong(SAM_DATABASE_DOMAIN));
	PyModule_AddObject(m, "SV_TYPE_DIALIN_SERVER", PyInt_FromLong(SV_TYPE_DIALIN_SERVER));
	PyModule_AddObject(m, "SEC_CHAN_LOCAL", PyInt_FromLong(SEC_CHAN_LOCAL));
	PyModule_AddObject(m, "REG_NONE", PyInt_FromLong(REG_NONE));
	PyModule_AddObject(m, "SV_TYPE_SERVER_UNIX", PyInt_FromLong(SV_TYPE_SERVER_UNIX));
	PyModule_AddObject(m, "SV_TYPE_ALL", PyInt_FromLong(0xFFFFFFFF));
	PyModule_AddObject(m, "SV_TYPE_TIME_SOURCE", PyInt_FromLong(SV_TYPE_TIME_SOURCE));
	PyModule_AddObject(m, "SV_TYPE_SERVER_MFPN", PyInt_FromLong(SV_TYPE_SERVER_MFPN));
	PyModule_AddObject(m, "REG_DWORD", PyInt_FromLong(REG_DWORD));
	PyModule_AddObject(m, "REG_RESOURCE_REQUIREMENTS_LIST", PyInt_FromLong(REG_RESOURCE_REQUIREMENTS_LIST));
	PyModule_AddObject(m, "SV_TYPE_DOMAIN_ENUM", PyInt_FromLong(SV_TYPE_DOMAIN_ENUM));
	PyModule_AddObject(m, "REG_RESOURCE_LIST", PyInt_FromLong(REG_RESOURCE_LIST));
	PyModule_AddObject(m, "SV_TYPE_ALTERNATE_XPORT", PyInt_FromLong(SV_TYPE_ALTERNATE_XPORT));
	PyModule_AddObject(m, "SV_TYPE_LOCAL_LIST_ONLY", PyInt_FromLong(SV_TYPE_LOCAL_LIST_ONLY));
	Py_INCREF((PyObject *)(void *)&GUID_Type);
	PyModule_AddObject(m, "GUID", (PyObject *)(void *)&GUID_Type);
	Py_INCREF((PyObject *)(void *)&ndr_syntax_id_Type);
	PyModule_AddObject(m, "ndr_syntax_id", (PyObject *)(void *)&ndr_syntax_id_Type);
	Py_INCREF((PyObject *)(void *)&policy_handle_Type);
	PyModule_AddObject(m, "policy_handle", (PyObject *)(void *)&policy_handle_Type);
	Py_INCREF((PyObject *)(void *)&KRB5_EDATA_NTSTATUS_Type);
	PyModule_AddObject(m, "KRB5_EDATA_NTSTATUS", (PyObject *)(void *)&KRB5_EDATA_NTSTATUS_Type);
#ifdef PY_MOD_MISC_PATCH
	PY_MOD_MISC_PATCH(m);
#endif

}
