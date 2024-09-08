#ifndef SHUTDOWNMANAGER_HPP
#define SHUTDOWNMANAGER_HPP

class ShutdownManager
{
public:
	static ShutdownManager& getInstance()
	{
		static ShutdownManager instance;
		return instance;
	}

	void setShutdownSignal(bool signal)
	{
		shutdown_signal = signal;
	}

	bool getShutdownSignal() const
	{
		return shutdown_signal;
	}

private:
	ShutdownManager() : shutdown_signal(false) {}
	~ShutdownManager() {}

	ShutdownManager(const ShutdownManager&);
	ShutdownManager& operator=(const ShutdownManager&);

	bool shutdown_signal;
};

#endif