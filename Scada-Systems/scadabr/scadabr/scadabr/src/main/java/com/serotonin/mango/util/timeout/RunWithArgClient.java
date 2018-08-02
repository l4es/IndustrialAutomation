package com.serotonin.mango.util.timeout;

public interface RunWithArgClient<T> {

    void run(T arg, long fireTime);
}
