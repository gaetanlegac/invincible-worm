namespace PC {
	static string getSystem32() {
		TCHAR systemBuff[MAX_PATH];
		GetSystemDirectory(systemBuff, MAX_PATH);

		string system32 = systemBuff;

		return system32 + '\\';
	}

	static string getTemp() {
		TCHAR Buff[MAX_PATH];
		GetTempPath(MAX_PATH, Buff);

		return Buff;
	}
}