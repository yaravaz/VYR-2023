#include "stdafx.h"

int wmain(int argc, wchar_t* argv[]) {
	setlocale(LC_ALL, "rus");
	Log::LOG log = Log::INITLOG;
	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getlog(parm.log);
		log.errors_cout = 0;
		if (parm.more)
			Log::WriteLine(&cout, "����: ", "��� ������ ", "");
		Log::WriteLog(log.stream);
		if (parm.more)
			Log::WriteLog(&cout);

		Log::WriteParm(log.stream, parm);
		if (parm.more)
			Log::WriteParm(&cout, parm);

		In::IN in = In::getin(parm.in);
		Log::WriteIn(log.stream, in);
		if (parm.more)
			Log::WriteIn(&cout, in);

		Lex::LEX lex = Lex::lexAnaliz(log, in);
		cout << "-----����������� ������ ����������\n\n";

		fstream fout;
		fout.open(parm.out, ios_base::app);
		if (!fout.is_open())
			throw ERROR_THROW(110);
		LT::showTable(lex.lextable, &fout);
		if (parm.more || parm.Lout)
			LT::showTable(lex.lextable, &cout);

		if (parm.more || parm.lt)
			LT::writeLexTable(&cout, lex.lextable);

		IT::showITable(lex.idtable, log.stream);
		if (parm.more || parm.it)
			IT::showITable(lex.idtable, &cout);

		MFST::Mfst mfst(lex.lextable, GRB::getGreibach());
		mfst.log = log;
		if (parm.more || parm.lenta) mfst.more = true;
		if (mfst.start()) {
			cout << "-----�������������� �������� ��� ������\n\n";
		}
		else {
			cout << "-----������� ������ � ����\n\n";
			return 0;
		}
		mfst.savededucation();
		mfst.printrules();
		if (Sem::SemAnaliz(lex.lextable, lex.idtable, log)) cout << "-----��������e���� ������ �������� ��� ������\n\n";
		else {
			Log::WriteLine(log.stream, "-----������������� ������ ��������� ������(�)\n", "");
			cout << "-----������������� ������ ��������� ������(�)\n\n-----���������� ��������� �����������\n\n";
			return 0;
		}

		bool polish_ok = Polish::StartPolish(lex);
		if (!polish_ok)
		{
			Log::WriteLine(log.stream, "-----������ ��� ������� �������������� ���������\n", "");
			cout << "-----������ ��� ������� �������������� ���������\n\n-----���������� ��������� �����������\n\n";
			return 0;
		}
		else cout << "-----�������������� ��������� ��������� ��� ������\n\n";

		fout << "\n-----��������������� ������� ������\n";
		LT::showTable(lex.lextable, &fout);
		if (parm.more || parm.Lout) {
			cout << "\n-----��������������� ������� ������\n";
			LT::showTable(lex.lextable, &cout);
		}
		Gener:: CodeGeneration(lex, parm, log);
		Log::Close(log);
		if (log.errors_cout > 0) throw Error::geterror(0);

		/*cout << "-----������ ���������������� ����\n" << endl;
		system("msbuild.exe ..\\ASM /t:build  /p:cfg=\"debug | x86 -v:q\"");
		system("..\\Debug\\ASM.exe");*/
		return 0;
	}
	catch (Error::ERROR e)
	{
		cout << "\n-----���������� ��������� �����������\n\n";
		return 0;
	}
}