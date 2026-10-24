# i-guess-that-counts

## Description
High-performance benchmarking of memory mapped files, open addressing hash sets and modern hashers.

### Key takeaways
- hashset + hasher
- multithreading - false sharing, threadpools
- hw cores / hyperthreading, cpu avg load
- partitioning / static partitioning
- simd
- affinity
- hp/thp, madvise, alignment, padding
- custom allocators - mimalloc/jemalloc
- 2 hashers and identity hasher basing only on hash uniquness
- string instead of string_view - sso vs soft page faults

### Future work
- custom flatset - no removing, mmprefetch
- fair load balancing between partitions/threads
- data batching

### Build & run
```
sudo apt install pipx
pipx ensurepath
(restart shell)
pipx install conan
conan create conan/libaio --version=0.3.113 # broken mirror
conan install . --output-folder=build --build=missing -pr:h=conan/profiles/gcc15 -pr:b=conan/profiles/gcc15
cmake --preset conan-release
cmake --build build
sudo sysctl -w kernel.perf_event_paranoid=-1 # enable performance monitoring
sudo sysctl -w vm.nr_hugepages=512 # enable huge pages
perf stat -r 20 -d ./build/wordsCounter ../dataset253854_100MB.txt
```

### Benchmark
Testing 100MB file with 253854 unique words on i7-8750H (6/12) @ 4.10 GHz. Results in milliseconds averaged across multiple runs.

<table>
  <thead>
    <tr>
      <th align="left">Container \ Hasher</th>
      <td align="center">Absl</td>
      <td align="center">Ankerl</td>
      <td align="center">XXH3</td>
      <td align="center">Rapid</td>
      <td align="center">Wy</td>
      <td align="center">Farm</td>
      <td align="center">Std</td>
      <td align="center">Folly</td>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>boost::unordered_flat_set</td>
      <td align="center" style="background-color: hsl(120, 85%, 82%);">110.155</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">114.733</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">114.939</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">116.582</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">117.070</td>
      <td align="center" style="background-color: hsl(110, 85%, 82%);">127.391</td>
      <td align="center" style="background-color: hsl(103, 85%, 82%);">139.814</td>
      <td align="center" style="background-color: hsl(87, 85%, 82%);">167.625</td>
    </tr>
    <tr>
      <td>phmap::flat_hash_set</td>
      <td align="center" style="background-color: hsl(120, 85%, 82%);">110.583</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">114.846</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">114.684</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">116.636</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">116.917</td>
      <td align="center" style="background-color: hsl(107, 85%, 82%);">132.327</td>
      <td align="center" style="background-color: hsl(99, 85%, 82%);">147.886</td>
      <td align="center" style="background-color: hsl(82, 85%, 82%);">178.324</td>
    </tr>
    <tr>
      <td>absl::flat_hash_set</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">114.580</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">117.432</td>
      <td align="center" style="background-color: hsl(117, 85%, 82%);">115.111</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">117.502</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">118.064</td>
      <td align="center" style="background-color: hsl(105, 85%, 82%);">136.757</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">150.146</td>
      <td align="center" style="background-color: hsl(76, 85%, 82%);">188.243</td>
    </tr>
    <tr>
      <td>folly::F14FastSet</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">116.732</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">119.006</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">118.006</td>
      <td align="center" style="background-color: hsl(116, 85%, 82%);">120.301</td>
      <td align="center" style="background-color: hsl(115, 85%, 82%);">120.575</td>
      <td align="center" style="background-color: hsl(104, 85%, 82%);">139.117</td>
      <td align="center" style="background-color: hsl(96, 85%, 82%);">151.789</td>
      <td align="center" style="background-color: hsl(78, 85%, 82%);">185.083</td>
    </tr>
    <tr>
      <td>ankerl::unordered_dense::set</td>
      <td align="center" style="background-color: hsl(115, 85%, 82%);">120.916</td>
      <td align="center" style="background-color: hsl(113, 85%, 82%);">124.770</td>
      <td align="center" style="background-color: hsl(114, 85%, 82%);">122.955</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">126.974</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">126.230</td>
      <td align="center" style="background-color: hsl(101, 85%, 82%);">145.196</td>
      <td align="center" style="background-color: hsl(95, 85%, 82%);">155.068</td>
      <td align="center" style="background-color: hsl(80, 85%, 82%);">181.218</td>
    </tr>
    <tr>
      <td>phmap::parallel_flat_hash_set</td>
      <td align="center" style="background-color: hsl(115, 85%, 82%);">122.096</td>
      <td align="center" style="background-color: hsl(113, 85%, 82%);">125.688</td>
      <td align="center" style="background-color: hsl(113, 85%, 82%);">124.965</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">127.151</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">127.185</td>
      <td align="center" style="background-color: hsl(99, 85%, 82%);">148.016</td>
      <td align="center" style="background-color: hsl(94, 85%, 82%);">156.963</td>
      <td align="center" style="background-color: hsl(74, 85%, 82%);">193.364</td>
    </tr>
    <tr>
      <td>robin_hood::unordered_flat_set</td>
      <td align="center" style="background-color: hsl(115, 85%, 82%);">122.399</td>
      <td align="center" style="background-color: hsl(113, 85%, 82%);">125.655</td>
      <td align="center" style="background-color: hsl(108, 85%, 82%);">135.210</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">126.319</td>
      <td align="center" style="background-color: hsl(112, 85%, 82%);">126.046</td>
      <td align="center" style="background-color: hsl(100, 85%, 82%);">147.241</td>
      <td align="center" style="background-color: hsl(92, 85%, 82%);">161.428</td>
      <td align="center" style="background-color: hsl(81, 85%, 82%);">180.709</td>
    </tr>
    <tr>
      <td>tsl::robin_set</td>
      <td align="center" style="background-color: hsl(108, 85%, 82%);">133.568</td>
      <td align="center" style="background-color: hsl(108, 85%, 82%);">134.428</td>
      <td align="center" style="background-color: hsl(108, 85%, 82%);">133.407</td>
      <td align="center" style="background-color: hsl(107, 85%, 82%);">136.216</td>
      <td align="center" style="background-color: hsl(107, 85%, 82%);">136.242</td>
      <td align="center" style="background-color: hsl(99, 85%, 82%);">150.311</td>
      <td align="center" style="background-color: hsl(92, 85%, 82%);">161.350</td>
      <td align="center" style="background-color: hsl(82, 85%, 82%);">178.694</td>
    </tr>
    <tr>
      <td>tsl::sparse_set</td>
      <td align="center" style="background-color: hsl(105, 85%, 82%);">139.117</td>
      <td align="center" style="background-color: hsl(104, 85%, 82%);">141.056</td>
      <td align="center" style="background-color: hsl(101, 85%, 82%);">147.112</td>
      <td align="center" style="background-color: hsl(104, 85%, 82%);">142.470</td>
      <td align="center" style="background-color: hsl(104, 85%, 82%);">142.408</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">153.840</td>
      <td align="center" style="background-color: hsl(91, 85%, 82%);">167.318</td>
      <td align="center" style="background-color: hsl(80, 85%, 82%);">183.355</td>
    </tr>
    <tr>
      <td>tsl::bhopscotch_set</td>
      <td align="center" style="background-color: hsl(104, 85%, 82%);">141.724</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">154.279</td>
      <td align="center" style="background-color: hsl(98, 85%, 82%);">151.751</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">154.896</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">154.200</td>
      <td align="center" style="background-color: hsl(91, 85%, 82%);">165.731</td>
      <td align="center" style="background-color: hsl(87, 85%, 82%);">176.620</td>
      <td align="center" style="background-color: hsl(72, 85%, 82%);">203.240</td>
    </tr>
    <tr>
      <td>tsl::hopscotch_set</td>
      <td align="center" style="background-color: hsl(103, 85%, 82%);">143.702</td>
      <td align="center" style="background-color: hsl(96, 85%, 82%);">155.105</td>
      <td align="center" style="background-color: hsl(98, 85%, 82%);">152.016</td>
      <td align="center" style="background-color: hsl(96, 85%, 82%);">156.417</td>
      <td align="center" style="background-color: hsl(93, 85%, 82%);">161.940</td>
      <td align="center" style="background-color: hsl(91, 85%, 82%);">166.425</td>
      <td align="center" style="background-color: hsl(88, 85%, 82%);">173.344</td>
      <td align="center" style="background-color: hsl(75, 85%, 82%);">196.223</td>
    </tr>
    <tr>
      <td>boost::nordered_node_set</td>
      <td align="center" style="background-color: hsl(101, 85%, 82%);">146.402</td>
      <td align="center" style="background-color: hsl(98, 85%, 82%);">152.540</td>
      <td align="center" style="background-color: hsl(98, 85%, 82%);">151.782</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">153.250</td>
      <td align="center" style="background-color: hsl(97, 85%, 82%);">153.682</td>
      <td align="center" style="background-color: hsl(91, 85%, 82%);">164.887</td>
      <td align="center" style="background-color: hsl(88, 85%, 82%);">173.190</td>
      <td align="center" style="background-color: hsl(73, 85%, 82%);">200.540</td>
    </tr>
    <tr>
      <td>phmap::node_hash_set</td>
      <td align="center" style="background-color: hsl(99, 85%, 82%);">149.882</td>
      <td align="center" style="background-color: hsl(95, 85%, 82%);">156.634</td>
      <td align="center" style="background-color: hsl(96, 85%, 82%);">154.212</td>
      <td align="center" style="background-color: hsl(95, 85%, 82%);">158.423</td>
      <td align="center" style="background-color: hsl(94, 85%, 82%);">160.756</td>
      <td align="center" style="background-color: hsl(84, 85%, 82%);">178.694</td>
      <td align="center" style="background-color: hsl(74, 85%, 82%);">196.883</td>
      <td align="center" style="background-color: hsl(64, 85%, 82%);">224.286</td>
    </tr>
    <tr>
      <td>phmap::parallel_node_hash_set</td>
      <td align="center" style="background-color: hsl(94, 85%, 82%);">163.468</td>
      <td align="center" style="background-color: hsl(93, 85%, 82%);">165.733</td>
      <td align="center" style="background-color: hsl(94, 85%, 82%);">164.225</td>
      <td align="center" style="background-color: hsl(92, 85%, 82%);">168.082</td>
      <td align="center" style="background-color: hsl(92, 85%, 82%);">168.791</td>
      <td align="center" style="background-color: hsl(81, 85%, 82%);">188.790</td>
      <td align="center" style="background-color: hsl(71, 85%, 82%);">206.592</td>
      <td align="center" style="background-color: hsl(64, 85%, 82%);">224.512</td>
    </tr>
    <tr>
      <td>boost::unordered_set</td>
      <td align="center" style="background-color: hsl(79, 85%, 82%);">200.741</td>
      <td align="center" style="background-color: hsl(79, 85%, 82%);">201.764</td>
      <td align="center" style="background-color: hsl(74, 85%, 82%);">211.397</td>
      <td align="center" style="background-color: hsl(76, 85%, 82%);">205.698</td>
      <td align="center" style="background-color: hsl(74, 85%, 82%);">210.875</td>
      <td align="center" style="background-color: hsl(71, 85%, 82%);">217.347</td>
      <td align="center" style="background-color: hsl(67, 85%, 82%);">227.126</td>
      <td align="center" style="background-color: hsl(59, 85%, 82%);">248.804</td>
    </tr>
    <tr>
      <td>std::unordered_set</td>
      <td align="center" style="background-color: hsl(50, 85%, 82%);">253.284</td>
      <td align="center" style="background-color: hsl(46, 85%, 82%);">261.272</td>
      <td align="center" style="background-color: hsl(46, 85%, 82%);">262.158</td>
      <td align="center" style="background-color: hsl(45, 85%, 82%);">262.774</td>
      <td align="center" style="background-color: hsl(42, 85%, 82%);">270.812</td>
      <td align="center" style="background-color: hsl(42, 85%, 82%);">270.218</td>
      <td align="center" style="background-color: hsl(28, 85%, 82%);">306.918</td>
      <td align="center" style="background-color: hsl(0, 85%, 82%);">320.287</td>
    </tr>
  </tbody>
</table>
