namespace android {

class WrapperDeathObserver : public IBinder::DeathRecipient {
	sp<IWrapper> wrapper;
	public:
	virtual void binderDied(const wp<IBinder>& who) {
		//handle Taintgrind disappearing
		ALOGD("wrapper service has died :(");
		wrapper->setServiceState(false);
	}

	void setWrapper(sp<IWrapper> newWrapper) {
		wrapper = newWrapper;
	}
};


}
