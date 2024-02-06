# 変更点
## fft_engine.hpp
ここに定義されている以下のコードにて、ポインタ渡しされた変数に対して、size()メソッドを呼び出してエラー吐くので、コメントアウトしました。  
```cpp
template <typename T>
void FFTEngine<T>::fft(std::complex<T> *a)
{
    assert(static_cast<int>(a.size()) == m_frameSize); // ポインタ.size()だから動かんかも。
    cdft(m_frameSize * 2, -1, reinterpret_cast<T *>(a), &m_ip[0], &m_w[0]);
}
```

ビルドの方法や実装の編集でどうにかなるようであれば、今後の本リポジトリ内で更新します。  