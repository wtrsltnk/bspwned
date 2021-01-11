#ifndef _INDEXARRAY_H
#define	_INDEXARRAY_H

template<const int elem_size>
class array_t {
public:
#define CHUNK   (4096)
    array_t() {
        size = CHUNK*elem_size;
        data = new float[size];
        count = 0;
    }

    ~array_t() {
        if(data) {
            delete []data;
            data = 0;
        }
    }

    void add(float* src) {
        if(count >= size/elem_size) {
            //resize
            int s = size;
            size += CHUNK*elem_size;
            float* n = new float[size];
            for(int i = 0; i < s; i++) {
                n[i] = data[i];
            }
            delete data;
            data = n;
        }

        float* dst = data + count*elem_size;
        for(int i = 0; i < elem_size; i++) {
            *dst++ = *src++;
        }
        count++;
    }

    operator float*(void) const { return data; }
    const float* operator[](int i) const { return data + i*elem_size; }

    int current() const { return count; }

private:
    int size;
    int count;
    float* data;
};

#endif	/* _INDEXARRAY_H */

