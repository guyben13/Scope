
## Run Took 0:00:14.000


### Profiling Tree

<details><summary>
  <span style="color: red;" title="71% self">100%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:4&#10;func1">func1:4</span>
  <span style="color: green;" title="0:00:10.000 self">0:00:14.000</span>
  <span style="color: orange;" title="1.000000 secs. each">for 14 times</span>
</summary><blockquote>
  <p><span style="color: red;" title="28% self">28%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
  <span style="color: green;" title="0:00:04.000 self">0:00:04.000</span>
  <span style="color: orange;" title="0.333333 secs. each">for 12 times</span></p>
  <details><summary>
    <span style="font-style: italic;">group</span>
  </summary><blockquote>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.000002 secs. each">for 1,000,000 times</span></p>
  </blockquote></details>
</blockquote></details>

### Per Function Summary

<details><summary>
  <span style="font-style: italic;">Summary - ordered by total time</span>
</summary><blockquote>
  <details><summary>
    <span style="color: red;" title="71% self">100%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:4&#10;func1">func1:4</span>
    <span style="color: green;" title="0:00:10.000 self">0:00:14.000</span>
    <span style="color: orange;" title="1.000000 secs. each">for 14 times</span>
  </summary><blockquote>
    <p><span style="color: red;" title="28% self">28%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
    <span style="color: green;" title="0:00:04.000 self">0:00:04.000</span>
    <span style="color: orange;" title="0.333333 secs. each">for 12 times</span></p>
    <details><summary>
      <span style="font-style: italic;">group</span>
    </summary><blockquote>
      <p><span style="color: red;" title="14% self">14%</span>
      <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
      <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
      <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
      <p><span style="color: red;" title="14% self">14%</span>
      <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
      <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
      <span style="color: orange;" title="0.000002 secs. each">for 1,000,000 times</span></p>
    </blockquote></details>
  </blockquote></details>
  <p><span style="color: red;" title="42% self">42%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
  <span style="color: green;" title="0:00:06.000 self">0:00:06.000</span>
  <span style="color: orange;" title="0.000006 secs. each">for 1,000,012 times</span></p>
  <p><span style="color: red;" title="14% self">14%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
  <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
  <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
  <details><summary>
    <span style="font-style: italic;">group</span>
  </summary><blockquote>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.000002 secs. each">for 1,000,000 times</span></p>
  </blockquote></details>
</blockquote></details>
<details><summary>
  <span style="font-style: italic;">Summary - ordered by self time</span>
</summary><blockquote>
  <details><summary>
    <span style="color: red;" title="71% self">100%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:4&#10;func1">func1:4</span>
    <span style="color: green;" title="0:00:10.000 self">0:00:14.000</span>
    <span style="color: orange;" title="1.000000 secs. each">for 14 times</span>
  </summary><blockquote>
    <p><span style="color: red;" title="28% self">28%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
    <span style="color: green;" title="0:00:04.000 self">0:00:04.000</span>
    <span style="color: orange;" title="0.333333 secs. each">for 12 times</span></p>
    <details><summary>
      <span style="font-style: italic;">group</span>
    </summary><blockquote>
      <p><span style="color: red;" title="14% self">14%</span>
      <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
      <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
      <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
      <p><span style="color: red;" title="14% self">14%</span>
      <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
      <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
      <span style="color: orange;" title="0.000002 secs. each">for 1,000,000 times</span></p>
    </blockquote></details>
  </blockquote></details>
  <p><span style="color: red;" title="42% self">42%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
  <span style="color: green;" title="0:00:06.000 self">0:00:06.000</span>
  <span style="color: orange;" title="0.000006 secs. each">for 1,000,012 times</span></p>
  <p><span style="color: red;" title="14% self">14%</span>
  <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
  <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
  <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
  <details><summary>
    <span style="font-style: italic;">group</span>
  </summary><blockquote>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:12345&#10;func4">func4:12345</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.666667 secs. each">for 3 times</span></p>
    <p><span style="color: red;" title="14% self">14%</span>
    <span style="font-family: monospace; color: black;" title="f1.cc:8&#10;func2">func2:8</span>
    <span style="color: green;" title="0:00:02.000 self">0:00:02.000</span>
    <span style="color: orange;" title="0.000002 secs. each">for 1,000,000 times</span></p>
  </blockquote></details>
</blockquote></details>

