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
perf stat -r 20 -d ./build/wordsCounter --benchmark single --filename ../dataset253854_100MB.txt
[or]
./build/wordsCounter --benchmark full --filename ../dataset253854_100MB.txt

```

### Benchmark
Testing 100MB file with 253854 unique words on i7-8750H (6/12) @ 4.10 GHz. Results in milliseconds averaged across multiple runs.

<table>
<thead>
<tr>
<th align="left">Container \ Hasher</th>
<th align="center">Absl</th>
<th align="center">Ankerl</th>
<th align="center">XXH3</th>
<th align="center">Rapid</th>
<th align="center">Wy</th>
<th align="center">Farm</th>
<th align="center">Std</th>
<th align="center">Folly</th>
</tr>
</thead>
<tbody>
<tr>
<td>boost::unordered_flat_set</td>
<td align="center"><img src="https://img.shields.io/badge/110.155-22c55e?style=flat-square" alt="110.155"></td>
<td align="center"><img src="https://img.shields.io/badge/114.733-22c55e?style=flat-square" alt="114.733"></td>
<td align="center"><img src="https://img.shields.io/badge/114.939-22c55e?style=flat-square" alt="114.939"></td>
<td align="center"><img src="https://img.shields.io/badge/116.582-22c55e?style=flat-square" alt="116.582"></td>
<td align="center"><img src="https://img.shields.io/badge/117.070-22c55e?style=flat-square" alt="117.070"></td>
<td align="center"><img src="https://img.shields.io/badge/127.391-4ade80?style=flat-square" alt="127.391"></td>
<td align="center"><img src="https://img.shields.io/badge/139.814-84cc16?style=flat-square" alt="139.814"></td>
<td align="center"><img src="https://img.shields.io/badge/167.625-eab308?style=flat-square" alt="167.625"></td>
</tr>
<tr>
<td>phmap::flat_hash_set</td>
<td align="center"><img src="https://img.shields.io/badge/110.583-22c55e?style=flat-square" alt="110.583"></td>
<td align="center"><img src="https://img.shields.io/badge/114.846-22c55e?style=flat-square" alt="114.846"></td>
<td align="center"><img src="https://img.shields.io/badge/114.684-22c55e?style=flat-square" alt="114.684"></td>
<td align="center"><img src="https://img.shields.io/badge/116.636-22c55e?style=flat-square" alt="116.636"></td>
<td align="center"><img src="https://img.shields.io/badge/116.917-22c55e?style=flat-square" alt="116.917"></td>
<td align="center"><img src="https://img.shields.io/badge/132.327-4ade80?style=flat-square" alt="132.327"></td>
<td align="center"><img src="https://img.shields.io/badge/147.886-84cc16?style=flat-square" alt="147.886"></td>
<td align="center"><img src="https://img.shields.io/badge/178.324-eab308?style=flat-square" alt="178.324"></td>
</tr>
<tr>
<td>absl::flat_hash_set</td>
<td align="center"><img src="https://img.shields.io/badge/114.580-22c55e?style=flat-square" alt="114.580"></td>
<td align="center"><img src="https://img.shields.io/badge/117.432-22c55e?style=flat-square" alt="117.432"></td>
<td align="center"><img src="https://img.shields.io/badge/115.111-22c55e?style=flat-square" alt="115.111"></td>
<td align="center"><img src="https://img.shields.io/badge/117.502-22c55e?style=flat-square" alt="117.502"></td>
<td align="center"><img src="https://img.shields.io/badge/118.064-22c55e?style=flat-square" alt="118.064"></td>
<td align="center"><img src="https://img.shields.io/badge/136.757-84cc16?style=flat-square" alt="136.757"></td>
<td align="center"><img src="https://img.shields.io/badge/150.146-84cc16?style=flat-square" alt="150.146"></td>
<td align="center"><img src="https://img.shields.io/badge/188.243-f59e0b?style=flat-square" alt="188.243"></td>
</tr>
<tr>
<td>folly::F14FastSet</td>
<td align="center"><img src="https://img.shields.io/badge/116.732-22c55e?style=flat-square" alt="116.732"></td>
<td align="center"><img src="https://img.shields.io/badge/119.006-22c55e?style=flat-square" alt="119.006"></td>
<td align="center"><img src="https://img.shields.io/badge/118.006-22c55e?style=flat-square" alt="118.006"></td>
<td align="center"><img src="https://img.shields.io/badge/120.301-22c55e?style=flat-square" alt="120.301"></td>
<td align="center"><img src="https://img.shields.io/badge/120.575-22c55e?style=flat-square" alt="120.575"></td>
<td align="center"><img src="https://img.shields.io/badge/139.117-84cc16?style=flat-square" alt="139.117"></td>
<td align="center"><img src="https://img.shields.io/badge/151.789-84cc16?style=flat-square" alt="151.789"></td>
<td align="center"><img src="https://img.shields.io/badge/185.083-f59e0b?style=flat-square" alt="185.083"></td>
</tr>
<tr>
<td>ankerl::unordered_dense::set</td>
<td align="center"><img src="https://img.shields.io/badge/120.916-22c55e?style=flat-square" alt="120.916"></td>
<td align="center"><img src="https://img.shields.io/badge/124.770-22c55e?style=flat-square" alt="124.770"></td>
<td align="center"><img src="https://img.shields.io/badge/122.955-22c55e?style=flat-square" alt="122.955"></td>
<td align="center"><img src="https://img.shields.io/badge/126.974-4ade80?style=flat-square" alt="126.974"></td>
<td align="center"><img src="https://img.shields.io/badge/126.230-4ade80?style=flat-square" alt="126.230"></td>
<td align="center"><img src="https://img.shields.io/badge/145.196-84cc16?style=flat-square" alt="145.196"></td>
<td align="center"><img src="https://img.shields.io/badge/155.068-84cc16?style=flat-square" alt="155.068"></td>
<td align="center"><img src="https://img.shields.io/badge/181.218-f59e0b?style=flat-square" alt="181.218"></td>
</tr>
<tr>
<td>phmap::parallel_flat_hash_set</td>
<td align="center"><img src="https://img.shields.io/badge/122.096-22c55e?style=flat-square" alt="122.096"></td>
<td align="center"><img src="https://img.shields.io/badge/125.688-4ade80?style=flat-square" alt="125.688"></td>
<td align="center"><img src="https://img.shields.io/badge/124.965-22c55e?style=flat-square" alt="124.965"></td>
<td align="center"><img src="https://img.shields.io/badge/127.151-4ade80?style=flat-square" alt="127.151"></td>
<td align="center"><img src="https://img.shields.io/badge/127.185-4ade80?style=flat-square" alt="127.185"></td>
<td align="center"><img src="https://img.shields.io/badge/148.016-84cc16?style=flat-square" alt="148.016"></td>
<td align="center"><img src="https://img.shields.io/badge/156.963-84cc16?style=flat-square" alt="156.963"></td>
<td align="center"><img src="https://img.shields.io/badge/193.364-f59e0b?style=flat-square" alt="193.364"></td>
</tr>
<tr>
<td>robin_hood::unordered_flat_set</td>
<td align="center"><img src="https://img.shields.io/badge/122.399-22c55e?style=flat-square" alt="122.399"></td>
<td align="center"><img src="https://img.shields.io/badge/125.655-4ade80?style=flat-square" alt="125.655"></td>
<td align="center"><img src="https://img.shields.io/badge/135.210-84cc16?style=flat-square" alt="135.210"></td>
<td align="center"><img src="https://img.shields.io/badge/126.319-4ade80?style=flat-square" alt="126.319"></td>
<td align="center"><img src="https://img.shields.io/badge/126.046-4ade80?style=flat-square" alt="126.046"></td>
<td align="center"><img src="https://img.shields.io/badge/147.241-84cc16?style=flat-square" alt="147.241"></td>
<td align="center"><img src="https://img.shields.io/badge/161.428-eab308?style=flat-square" alt="161.428"></td>
<td align="center"><img src="https://img.shields.io/badge/180.709-f59e0b?style=flat-square" alt="180.709"></td>
</tr>
<tr>
<td>tsl::robin_set</td>
<td align="center"><img src="https://img.shields.io/badge/133.568-84cc16?style=flat-square" alt="133.568"></td>
<td align="center"><img src="https://img.shields.io/badge/134.428-84cc16?style=flat-square" alt="134.428"></td>
<td align="center"><img src="https://img.shields.io/badge/133.407-84cc16?style=flat-square" alt="133.407"></td>
<td align="center"><img src="https://img.shields.io/badge/136.216-84cc16?style=flat-square" alt="136.216"></td>
<td align="center"><img src="https://img.shields.io/badge/136.242-84cc16?style=flat-square" alt="136.242"></td>
<td align="center"><img src="https://img.shields.io/badge/150.311-84cc16?style=flat-square" alt="150.311"></td>
<td align="center"><img src="https://img.shields.io/badge/161.350-eab308?style=flat-square" alt="161.350"></td>
<td align="center"><img src="https://img.shields.io/badge/178.694-f59e0b?style=flat-square" alt="178.694"></td>
</tr>
<tr>
<td>tsl::sparse_set</td>
<td align="center"><img src="https://img.shields.io/badge/139.117-84cc16?style=flat-square" alt="139.117"></td>
<td align="center"><img src="https://img.shields.io/badge/141.056-84cc16?style=flat-square" alt="141.056"></td>
<td align="center"><img src="https://img.shields.io/badge/147.112-84cc16?style=flat-square" alt="147.112"></td>
<td align="center"><img src="https://img.shields.io/badge/142.470-84cc16?style=flat-square" alt="142.470"></td>
<td align="center"><img src="https://img.shields.io/badge/142.408-84cc16?style=flat-square" alt="142.408"></td>
<td align="center"><img src="https://img.shields.io/badge/153.840-84cc16?style=flat-square" alt="153.840"></td>
<td align="center"><img src="https://img.shields.io/badge/167.318-eab308?style=flat-square" alt="167.318"></td>
<td align="center"><img src="https://img.shields.io/badge/183.355-f59e0b?style=flat-square" alt="183.355"></td>
</tr>
<tr>
<td>tsl::bhopscotch_set</td>
<td align="center"><img src="https://img.shields.io/badge/141.724-84cc16?style=flat-square" alt="141.724"></td>
<td align="center"><img src="https://img.shields.io/badge/154.279-84cc16?style=flat-square" alt="154.279"></td>
<td align="center"><img src="https://img.shields.io/badge/151.751-84cc16?style=flat-square" alt="151.751"></td>
<td align="center"><img src="https://img.shields.io/badge/154.896-84cc16?style=flat-square" alt="154.896"></td>
<td align="center"><img src="https://img.shields.io/badge/154.200-84cc16?style=flat-square" alt="154.200"></td>
<td align="center"><img src="https://img.shields.io/badge/165.731-eab308?style=flat-square" alt="165.731"></td>
<td align="center"><img src="https://img.shields.io/badge/176.620-eab308?style=flat-square" alt="176.620"></td>
<td align="center"><img src="https://img.shields.io/badge/203.240-f59e0b?style=flat-square" alt="203.240"></td>
</tr>
<tr>
<td>tsl::hopscotch_set</td>
<td align="center"><img src="https://img.shields.io/badge/143.702-84cc16?style=flat-square" alt="143.702"></td>
<td align="center"><img src="https://img.shields.io/badge/155.105-84cc16?style=flat-square" alt="155.105"></td>
<td align="center"><img src="https://img.shields.io/badge/152.016-84cc16?style=flat-square" alt="152.016"></td>
<td align="center"><img src="https://img.shields.io/badge/156.417-84cc16?style=flat-square" alt="156.417"></td>
<td align="center"><img src="https://img.shields.io/badge/161.940-eab308?style=flat-square" alt="161.940"></td>
<td align="center"><img src="https://img.shields.io/badge/166.425-eab308?style=flat-square" alt="166.425"></td>
<td align="center"><img src="https://img.shields.io/badge/173.344-eab308?style=flat-square" alt="173.344"></td>
<td align="center"><img src="https://img.shields.io/badge/196.223-f59e0b?style=flat-square" alt="196.223"></td>
</tr>
<tr>
<td>boost::unordered_node_set</td>
<td align="center"><img src="https://img.shields.io/badge/146.402-84cc16?style=flat-square" alt="146.402"></td>
<td align="center"><img src="https://img.shields.io/badge/152.540-84cc16?style=flat-square" alt="152.540"></td>
<td align="center"><img src="https://img.shields.io/badge/151.782-84cc16?style=flat-square" alt="151.782"></td>
<td align="center"><img src="https://img.shields.io/badge/153.250-84cc16?style=flat-square" alt="153.250"></td>
<td align="center"><img src="https://img.shields.io/badge/153.682-84cc16?style=flat-square" alt="153.682"></td>
<td align="center"><img src="https://img.shields.io/badge/164.887-eab308?style=flat-square" alt="164.887"></td>
<td align="center"><img src="https://img.shields.io/badge/173.190-eab308?style=flat-square" alt="173.190"></td>
<td align="center"><img src="https://img.shields.io/badge/200.540-f59e0b?style=flat-square" alt="200.540"></td>
</tr>
<tr>
<td>phmap::node_hash_set</td>
<td align="center"><img src="https://img.shields.io/badge/149.882-84cc16?style=flat-square" alt="149.882"></td>
<td align="center"><img src="https://img.shields.io/badge/156.634-84cc16?style=flat-square" alt="156.634"></td>
<td align="center"><img src="https://img.shields.io/badge/154.212-84cc16?style=flat-square" alt="154.212"></td>
<td align="center"><img src="https://img.shields.io/badge/158.423-84cc16?style=flat-square" alt="158.423"></td>
<td align="center"><img src="https://img.shields.io/badge/160.756-eab308?style=flat-square" alt="160.756"></td>
<td align="center"><img src="https://img.shields.io/badge/178.694-f59e0b?style=flat-square" alt="178.694"></td>
<td align="center"><img src="https://img.shields.io/badge/196.883-f59e0b?style=flat-square" alt="196.883"></td>
<td align="center"><img src="https://img.shields.io/badge/224.286-ef4444?style=flat-square" alt="224.286"></td>
</tr>
<tr>
<td>phmap::parallel_node_hash_set</td>
<td align="center"><img src="https://img.shields.io/badge/163.468-eab308?style=flat-square" alt="163.468"></td>
<td align="center"><img src="https://img.shields.io/badge/165.733-eab308?style=flat-square" alt="165.733"></td>
<td align="center"><img src="https://img.shields.io/badge/164.225-eab308?style=flat-square" alt="164.225"></td>
<td align="center"><img src="https://img.shields.io/badge/168.082-eab308?style=flat-square" alt="168.082"></td>
<td align="center"><img src="https://img.shields.io/badge/168.791-eab308?style=flat-square" alt="168.791"></td>
<td align="center"><img src="https://img.shields.io/badge/188.790-f59e0b?style=flat-square" alt="188.790"></td>
<td align="center"><img src="https://img.shields.io/badge/206.592-f59e0b?style=flat-square" alt="206.592"></td>
<td align="center"><img src="https://img.shields.io/badge/224.512-ef4444?style=flat-square" alt="224.512"></td>
</tr>
<tr>
<td>boost::unordered_set</td>
<td align="center"><img src="https://img.shields.io/badge/200.741-f59e0b?style=flat-square" alt="200.741"></td>
<td align="center"><img src="https://img.shields.io/badge/201.764-f59e0b?style=flat-square" alt="201.764"></td>
<td align="center"><img src="https://img.shields.io/badge/211.397-f59e0b?style=flat-square" alt="211.397"></td>
<td align="center"><img src="https://img.shields.io/badge/205.698-f59e0b?style=flat-square" alt="205.698"></td>
<td align="center"><img src="https://img.shields.io/badge/210.875-f59e0b?style=flat-square" alt="210.875"></td>
<td align="center"><img src="https://img.shields.io/badge/217.347-f59e0b?style=flat-square" alt="217.347"></td>
<td align="center"><img src="https://img.shields.io/badge/227.126-ef4444?style=flat-square" alt="227.126"></td>
<td align="center"><img src="https://img.shields.io/badge/248.804-ef4444?style=flat-square" alt="248.804"></td>
</tr>
<tr>
<td>std::unordered_set</td>
<td align="center"><img src="https://img.shields.io/badge/253.284-ef4444?style=flat-square" alt="253.284"></td>
<td align="center"><img src="https://img.shields.io/badge/261.272-b91c1c?style=flat-square" alt="261.272"></td>
<td align="center"><img src="https://img.shields.io/badge/262.158-b91c1c?style=flat-square" alt="262.158"></td>
<td align="center"><img src="https://img.shields.io/badge/262.774-b91c1c?style=flat-square" alt="262.774"></td>
<td align="center"><img src="https://img.shields.io/badge/270.812-b91c1c?style=flat-square" alt="270.812"></td>
<td align="center"><img src="https://img.shields.io/badge/270.218-b91c1c?style=flat-square" alt="270.218"></td>
<td align="center"><img src="https://img.shields.io/badge/306.918-991b1b?style=flat-square" alt="306.918"></td>
<td align="center"><img src="https://img.shields.io/badge/320.287-7f1d1d?style=flat-square" alt="320.287"></td>
</tr>
</tbody>
</table>
