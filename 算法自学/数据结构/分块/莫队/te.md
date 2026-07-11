<svg viewBox="0 0 600 660" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <marker id="arrowGray" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="gray" opacity="0.5" />
    </marker>
    <marker id="arrowBlue" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="rgb(38,120,220)" />
    </marker>
    <marker id="arrowBlueDash" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="rgb(38,120,220)" opacity="0.7" />
    </marker>
    <style>
      .grid { stroke: gray; stroke-opacity: 0.22; stroke-width: 0.6; }
      .block { stroke: gray; stroke-opacity: 0.60; stroke-width: 1.5; stroke-dasharray: 6 3; }
      .axis { stroke: gray; stroke-opacity: 0.50; stroke-width: 1.2; }
      .alab { fill: gray; fill-opacity: 0.50; font-family: sans-serif; font-size: 14; font-style: italic; }
      .tick { fill: gray; stroke: gray; stroke-opacity: 0.45; font-family: sans-serif; font-size: 9; }
      .mpath { fill: none; stroke: rgb(38,120,220); stroke-width: 2.2; }
      .ppath { fill: none; stroke: rgb(38,120,220); stroke-width: 1.6; stroke-dasharray: 6 4; opacity: 0.75; }
      .drop { stroke: gray; stroke-opacity: 0.45; stroke-width: 1; stroke-dasharray: 4 3; }
      .node { fill: white; stroke: white; }
      .ntext { font-family: sans-serif; font-size: 9; fill: black; stroke: none; }
      .pproj { fill: none; stroke: rgb(38,120,220); stroke-opacity: 0.55; stroke-width: 1; r: 2.5; }
    </style>
  </defs>
  <g class="grid">
    <line x1="260" y1="450" x2="540" y2="450" /><line x1="240" y1="470" x2="520" y2="470" />
    <line x1="220" y1="490" x2="500" y2="490" /><line x1="200" y1="510" x2="480" y2="510" />
    <line x1="180" y1="530" x2="460" y2="530" /><line x1="160" y1="550" x2="440" y2="550" />
    <line x1="140" y1="570" x2="420" y2="570" /><line x1="120" y1="590" x2="400" y2="590" />
    <line x1="100" y1="610" x2="380" y2="610" />
    <line x1="280" y1="430" x2="100" y2="610" /><line x1="308" y1="430" x2="128" y2="610" />
    <line x1="336" y1="430" x2="156" y2="610" /><line x1="364" y1="430" x2="184" y2="610" />
    <line x1="392" y1="430" x2="212" y2="610" /><line x1="420" y1="430" x2="240" y2="610" />
    <line x1="448" y1="430" x2="268" y2="610" /><line x1="476" y1="430" x2="296" y2="610" />
    <line x1="504" y1="430" x2="324" y2="610" /><line x1="532" y1="430" x2="352" y2="610" />
    <line x1="560" y1="430" x2="380" y2="610" />
  </g>
  <g class="block">
    <line x1="220" y1="490" x2="500" y2="490" /><line x1="160" y1="550" x2="440" y2="550" />
    <line x1="364" y1="430" x2="184" y2="610" /><line x1="448" y1="430" x2="268" y2="610" />
    <line x1="532" y1="430" x2="352" y2="610" />
  </g>
  <g class="axis">
    <line x1="280" y1="430" x2="580" y2="430" marker-end="url(#arrowGray)" /><text x="588" y="435" class="alab">r</text>
    <line x1="280" y1="430" x2="280" y2="210" marker-end="url(#arrowGray)" /><text x="272" y="202" class="alab">t</text>
    <line x1="280" y1="430" x2="92" y2="618" marker-end="url(#arrowGray)" /><text x="78" y="628" class="alab">l</text>
  </g>
  <!-- r 轴刻度 -->
  <g class="tick" text-anchor="middle">
    <line x1="308" y1="430" x2="308" y2="434" /><text x="308" y="446">1</text>
    <line x1="336" y1="430" x2="336" y2="434" /><text x="336" y="446">2</text>
    <line x1="364" y1="430" x2="364" y2="434" /><text x="364" y="446">3</text>
    <line x1="392" y1="430" x2="392" y2="434" /><text x="392" y="446">4</text>
    <line x1="420" y1="430" x2="420" y2="434" /><text x="420" y="446">5</text>
    <line x1="448" y1="430" x2="448" y2="434" /><text x="448" y="446">6</text>
    <line x1="476" y1="430" x2="476" y2="434" /><text x="476" y="446">7</text>
    <line x1="504" y1="430" x2="504" y2="434" /><text x="504" y="446">8</text>
    <line x1="532" y1="430" x2="532" y2="434" /><text x="532" y="446">9</text>
    <line x1="560" y1="430" x2="560" y2="434" /><text x="560" y="446">10</text>
  </g>
  <!-- t 轴刻度 (至 t=7) -->
  <g class="tick" text-anchor="end">
    <line x1="276" y1="400" x2="280" y2="400" /><text x="274" y="404">1</text>
    <line x1="276" y1="370" x2="280" y2="370" /><text x="274" y="374">2</text>
    <line x1="276" y1="340" x2="280" y2="340" /><text x="274" y="344">3</text>
    <line x1="276" y1="310" x2="280" y2="310" /><text x="274" y="314">4</text>
    <line x1="276" y1="280" x2="280" y2="280" /><text x="274" y="284">5</text>
    <line x1="276" y1="250" x2="280" y2="250" /><text x="274" y="254">6</text>
    <line x1="276" y1="220" x2="280" y2="220" /><text x="274" y="224">7</text>
  </g>
  <!-- l 轴刻度：竖直向下 -->
  <g class="tick" text-anchor="middle">
    <line x1="260" y1="450" x2="260" y2="456" /><text x="260" y="468">1</text>
    <line x1="240" y1="470" x2="240" y2="476" /><text x="240" y="488">2</text>
    <line x1="220" y1="490" x2="220" y2="496" /><text x="220" y="508">3</text>
    <line x1="200" y1="510" x2="200" y2="516" /><text x="200" y="528">4</text>
    <line x1="180" y1="530" x2="180" y2="536" /><text x="180" y="548">5</text>
    <line x1="160" y1="550" x2="160" y2="556" /><text x="160" y="568">6</text>
    <line x1="140" y1="570" x2="140" y2="576" /><text x="140" y="588">7</text>
    <line x1="120" y1="590" x2="120" y2="596" /><text x="120" y="608">8</text>
    <line x1="100" y1="610" x2="100" y2="616" /><text x="100" y="628">9</text>
  </g>
  <g class="drop">
    <line x1="464" y1="290" x2="464" y2="470" />
    <line x1="228" y1="420" x2="228" y2="510" />
    <line x1="276" y1="370" x2="276" y2="490" />
    <line x1="348" y1="320" x2="348" y2="530" />
    <line x1="224" y1="510" x2="224" y2="570" />
    <line x1="372" y1="440" x2="372" y2="590" />
  </g>
  <g class="ppath" marker-end="url(#arrowBlueDash)">
    <line x1="464" y1="470" x2="228" y2="510" />
    <line x1="228" y1="510" x2="276" y2="490" />
    <line x1="276" y1="490" x2="348" y2="530" />
    <line x1="348" y1="530" x2="224" y2="570" />
    <line x1="224" y1="570" x2="372" y2="590" />
  </g>
  <g>
    <circle cx="464" cy="470" class="pproj" />
    <circle cx="228" cy="510" class="pproj" />
    <circle cx="276" cy="490" class="pproj" />
    <circle cx="348" cy="530" class="pproj" />
    <circle cx="224" cy="570" class="pproj" />
    <circle cx="372" cy="590" class="pproj" />
  </g>
  <g class="mpath" marker-end="url(#arrowBlue)">
    <line x1="464" y1="290" x2="228" y2="420" />
    <line x1="228" y1="420" x2="276" y2="370" />
    <line x1="276" y1="370" x2="348" y2="320" />
    <line x1="348" y1="320" x2="224" y2="510" />
    <line x1="224" y1="510" x2="372" y2="440" />
  </g>
  <g class="node">
    <circle cx="464" cy="290" r="4" /><text x="470" y="287" class="ntext">A</text>
    <circle cx="228" cy="420" r="4" /><text x="234" y="417" class="ntext">E</text>
    <circle cx="276" cy="370" r="4" /><text x="282" y="367" class="ntext">B</text>
    <circle cx="348" cy="320" r="4" /><text x="354" y="317" class="ntext">C</text>
    <circle cx="224" cy="510" r="4" /><text x="230" y="507" class="ntext">D</text>
    <circle cx="372" cy="440" r="4" /><text x="378" y="437" class="ntext">F</text>
  </g>
  <text x="290" y="645" font-family="sans-serif" font-weight="bold" font-size="14" text-anchor="middle" fill="gray">Mo with Update (normal order)</text>
</svg>