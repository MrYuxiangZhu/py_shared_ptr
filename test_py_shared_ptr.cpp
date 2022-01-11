// py_shared_ptr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "py_memory.h"

using namespace std;
using namespace Python;

void test1()
{
	std::cout << "test1" << std::endl;
	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> pList2 = PyList_New(0);
	py_shared_ptr<PyObject> pTuple = PyTuple_New(2);

	py_shared_ptr<PyObject, false> temp = PyLong_FromLong(1L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(2L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(3L);
	PyList_Append(pList1.get(), temp.get());

	temp = PyLong_FromLong(4L);
	PyList_Append(pList2.get(), temp.get());
	temp = PyLong_FromLong(5L);
	PyList_Append(pList2.get(), temp.get());
	temp = PyLong_FromLong(6L);
	PyList_Append(pList2.get(), temp.get());

	PyTuple_SetItem(pTuple.get(), 0, pList1.get());
	PyTuple_SetItem(pTuple.get(), 1, pList2.get());

	PyObject_Print(pList1.get(), stdout, 0);
	std::cout << std::endl;
	PyObject_Print(pList2.get(), stdout, 0);
	std::cout << std::endl;
	PyObject_Print(pTuple.get(), stdout, 0);
	std::cout << std::endl;

	std::cout << pList1.use_count() << std::endl;
	std::cout << pList1.use_count() << std::endl;
	std::cout << pTuple.use_count() << std::endl;
}

void test2()
{
	std::cout << "test2" << std::endl;

	char* buf = (char*)PyMem_Malloc(BUFSIZ); /* for I/O */
	sprintf_s(buf, BUFSIZ, "%s", "123");

	py_shared_ptr<PyObject> res = PyBytes_FromString(buf);

	PyObject_Print(res.get(), stdout, 0);
	std::cout << std::endl;

	char* buf1 = (char*)PyMem_Malloc(BUFSIZ); /* for I/O */
	py_shared_ptr<PyObject> value = PyLong_FromString(buf, &buf1, 0);
	PyObject_Print(value.get(), stdout, 0);
	std::cout << std::endl;

	int* pv = (int*)PyMem_Malloc(sizeof(int));
	*pv = 124;

	py_shared_ptr<PyObject> lvalue = PyLong_FromLong(*pv);
	PyObject_Print(lvalue.get(), stdout, 0);
	std::cout << std::endl;

	PyMem_Free(buf); /* allocated with PyMem_Malloc */
	PyMem_Free(pv);
}

void test3()
{
	std::cout << "test3" << std::endl;

	PyObject* pList1 = PyList_New(0);
	PyObject* temp = PyLong_FromLong(1L);
	PyList_Append(pList1, temp);
	temp = PyLong_FromLong(2L);
	PyList_Append(pList1, temp);
	temp = PyLong_FromLong(3L);
	PyList_Append(pList1, temp);

	Py_XDECREF(temp);

	PyObject* temp1 = PyList_GetItem(pList1, 1);
	Py_XDECREF(temp1);

	PyObject* temp2 = PyList_GetItem(pList1, 1);
	//std::cout << temp2->ob_refcnt << std::endl;
	PyObject_Print(temp2, stdout, 0);
	std::cout << std::endl;
	Py_XDECREF(temp2);
	//std::cout << pList1->ob_refcnt << std::endl;
	Py_XDECREF(pList1);
}

void test4()
{
	std::cout << "test4" << std::endl;

	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> temp = PyLong_FromLong(1L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(2L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(3L);
	PyList_Append(pList1.get(), temp.get());
	
	for (int i = 0; i < 3; ++i)
	{
		py_shared_ptr<PyObject> temp1 = PyList_GetItem(pList1.get(), 1);
		PyObject_Print(temp1.get(), stdout, 0);
		std::cout << std::endl;
	}
	
	PyObject_Print(pList1.get(), stdout, 0);
	std::cout << std::endl;
}

void test5()
{
	std::cout << "test5" << std::endl << std::endl;

	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> pTemp = Py_BuildValue("i", 1);
	PyList_Append(pList1.get(), pTemp.get());
	pTemp = Py_BuildValue("i", 2);
	PyList_Append(pList1.get(), pTemp.get());
	pTemp = Py_BuildValue("i", 3);
	PyList_Append(pList1.get(), pTemp.get());

	PyObject_Print(pTemp.get(), stdout, 0);
	std::cout << std::endl;
	PyObject_Print(pList1.get(), stdout, 0);
	std::cout << std::endl;
}

void test6()
{
	std::cout << "test6" << std::endl;

	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> pTemp = PyFloat_FromDouble(12);
	PyList_Append(pList1.get(), pTemp.get());
	PyObject_Print(pTemp.get(), stdout, 0);
	std::cout << std::endl;
	PyObject_Print(pList1.get(), stdout, 0);
	std::cout << std::endl;
}

void test7(py_shared_ptr<PyObject> pList1)
{
	std::cout << "test7" << std::endl;

	py_shared_ptr<PyObject> temp1 = PyList_GetItem(pList1.get(), 0);
	PyObject_Print(temp1.get(), stdout, 0);
	std::cout << std::endl;
}

void test8()
{
	std::cout << "test8" << std::endl;

	py_shared_ptr<PyObject> pList = PyList_New(0);
	py_shared_ptr<PyObject> temp = PyLong_FromLong(1L);
	PyList_Append(pList.get(), temp.get());
	test7(pList);
}

void test9()
{
	std::cout << "test9" << std::endl;

	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> temp = PyLong_FromLong(1L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(2L);
	PyList_Append(pList1.get(), temp.get());
	temp = PyLong_FromLong(3L);
	PyList_Append(pList1.get(), temp.get());
	test7(pList1);
}

void test11(PyObject* pList1)
{
	std::cout << "test11" << std::endl;

	for (int cnt = 0; cnt < 3; ++cnt)
	{
		double dValue = 0.0;
		PyObject* pItem = ::PyList_GetItem(pList1, cnt);
		//std::cout << "ref " << pItem->ob_refcnt << std::endl;
		::PyArg_Parse(pItem, "d", &dValue);
		std::cout << dValue << std::endl;
		Py_XDECREF(pItem);
	}
}

void test10()
{
	std::cout << "test10" << std::endl;

	PyObject* pList1 = PyList_New(0);
	PyObject* temp = PyLong_FromLong(1L);
	PyList_Append(pList1, temp);
	temp = PyLong_FromLong(2L);
	PyList_Append(pList1, temp);
	temp = PyLong_FromLong(3L);
	PyList_Append(pList1, temp);
	Py_XDECREF(temp);
	test11(pList1);
	Py_XDECREF(pList1);
}

void test12()
{
	std::cout << "test12" << std::endl;

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	py_shared_ptr<PyObject> myModuleString = PyUnicode_FromString("interface");
	py_shared_ptr<PyObject> myModule = PyImport_Import(myModuleString.get());
	py_shared_ptr<PyObject> myFunction = PyObject_GetAttrString(myModule.get(), "add_numbers");

	py_shared_ptr<PyObject> pList1 = PyList_New(0);
	py_shared_ptr<PyObject> pList2 = PyList_New(0);

	for (int i = 1; i <= 10; ++i)
	{
		py_shared_ptr<PyObject> pTemp = PyLong_FromLong(i);
		PyList_Append(pList1.get(), pTemp.get());
		PyList_Append(pList2.get(), pTemp.get());
	}

	py_shared_ptr<PyObject> pParams = PyTuple_New(2);

	PyTuple_SetItem(pParams.get(), 0, pList1.get());
	PyTuple_SetItem(pParams.get(), 1, pList2.get());

	py_shared_ptr<PyObject> pResult = PyObject_CallObject(myFunction.get(), pParams.get());
	//std::cout << "ref " << pResult->ob_refcnt << std::endl;

	for (int cnt = 0; cnt < 10; ++cnt)
	{
		double dValue = 0.0;
		py_shared_ptr<PyObject> pTemp = PyList_GetItem(pResult.get(), cnt);
		PyArg_Parse(pTemp.get(), "d", &dValue);
		std::cout << dValue << " ";
	}

	std::cout << std::endl;
}

int main()
{
	Py_Initialize();

	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test8();
	test9();
	test10();
	test12();

	Py_FinalizeEx();

	return 0;
}
