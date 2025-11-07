#!/usr/bin/env python3
"""
Convert PROJECT_DOCUMENTATION.md to a presentation-ready PDF
Uses ReportLab to create a professional PDF document
"""

import os
from reportlab.lib.pagesizes import letter, A4
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import inch
from reportlab.lib.enums import TA_LEFT, TA_CENTER, TA_JUSTIFY
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, PageBreak, Table, TableStyle, Image
from reportlab.lib import colors
from datetime import datetime

# Output path
output_path = os.path.join(os.path.dirname(__file__), "Bhramanantdarshana_Presentation.pdf")

# Create PDF document
doc = SimpleDocTemplate(
    output_path,
    pagesize=letter,
    rightMargin=0.75*inch,
    leftMargin=0.75*inch,
    topMargin=0.75*inch,
    bottomMargin=0.75*inch,
)

# Create story (content list)
story = []

# Get styles
styles = getSampleStyleSheet()

# Define custom styles
title_style = ParagraphStyle(
    'CustomTitle',
    parent=styles['Heading1'],
    fontSize=28,
    textColor=colors.HexColor('#000000'),
    spaceAfter=12,
    alignment=TA_CENTER,
    fontName='Helvetica-Bold'
)

subtitle_style = ParagraphStyle(
    'CustomSubtitle',
    parent=styles['Heading2'],
    fontSize=16,
    textColor=colors.HexColor('#1a1a1a'),
    spaceAfter=8,
    alignment=TA_CENTER,
    fontName='Helvetica'
)

heading1_style = ParagraphStyle(
    'CustomHeading1',
    parent=styles['Heading1'],
    fontSize=18,
    textColor=colors.HexColor('#000000'),
    spaceAfter=10,
    spaceBefore=12,
    fontName='Helvetica-Bold'
)

heading2_style = ParagraphStyle(
    'CustomHeading2',
    parent=styles['Heading2'],
    fontSize=14,
    textColor=colors.HexColor('#1a1a1a'),
    spaceAfter=8,
    spaceBefore=10,
    fontName='Helvetica-Bold'
)

body_style = ParagraphStyle(
    'CustomBody',
    parent=styles['BodyText'],
    fontSize=11,
    alignment=TA_JUSTIFY,
    spaceAfter=6,
    leading=14
)

code_style = ParagraphStyle(
    'Code',
    parent=styles['BodyText'],
    fontSize=9,
    fontName='Courier',
    textColor=colors.HexColor('#333333'),
    spaceAfter=6,
    leftIndent=20
)

# === COVER PAGE ===
story.append(Spacer(1, 1.5*inch))

story.append(Paragraph("🌌 BHRAMANANTDARSHANA", title_style))
story.append(Spacer(1, 0.2*inch))

story.append(Paragraph("A Quantum Computing Education Platform", subtitle_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Complete Project Documentation & Presentation Guide", body_style))
story.append(Spacer(1, 0.8*inch))

# Project info table
info_data = [
    ["Project Type:", "Quantum Computing Simulator + Interactive Frontend"],
    ["Technology Stack:", "C++17 | Next.js 16 | React 19.2 | TypeScript | Tailwind CSS"],
    ["Status:", "Active Development"],
    ["Qubits Supported:", "1-5 (Frontend), n-qubit (Backend)"],
    ["Gates Implemented:", "15+"],
    ["Pre-built Templates:", "10+"],
    ["Date:", datetime.now().strftime("%B %d, %Y")]
]

info_table = Table(info_data, colWidths=[2*inch, 4*inch])
info_table.setStyle(TableStyle([
    ('BACKGROUND', (0, 0), (0, -1), colors.HexColor('#f0f0f0')),
    ('TEXTCOLOR', (0, 0), (-1, -1), colors.black),
    ('ALIGN', (0, 0), (-1, -1), 'LEFT'),
    ('FONTNAME', (0, 0), (0, -1), 'Helvetica-Bold'),
    ('FONTSIZE', (0, 0), (-1, -1), 11),
    ('BOTTOMPADDING', (0, 0), (-1, -1), 8),
    ('TOPPADDING', (0, 0), (-1, -1), 8),
    ('GRID', (0, 0), (-1, -1), 1, colors.HexColor('#cccccc')),
]))

story.append(info_table)
story.append(Spacer(1, 1*inch))

story.append(Paragraph("For presentation preparation by: Bhuvnesh", body_style))
story.append(Paragraph(f"Generated: {datetime.now().strftime('%B %d, %Y at %H:%M')}", body_style))

# Page break
story.append(PageBreak())

# === TABLE OF CONTENTS ===
story.append(Paragraph("📋 Table of Contents", heading1_style))
story.append(Spacer(1, 0.2*inch))

toc_items = [
    "1. Executive Overview",
    "2. Project Architecture",
    "3. Core Components",
    "4. Technology Stack",
    "5. Detailed Features",
    "6. User Guide",
    "7. Technical Implementation",
    "8. Installation & Deployment",
    "9. Future Roadmap",
    "10. Learning Outcomes",
]

for item in toc_items:
    story.append(Paragraph(f"• {item}", body_style))

story.append(PageBreak())

# === EXECUTIVE OVERVIEW ===
story.append(Paragraph("1. Executive Overview", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("What is Bhramanantdarshana?", heading2_style))
story.append(Paragraph(
    "Bhramanantdarshana (Sanskrit: 'witnessing the infinite circulation of cosmic energy') is a comprehensive "
    "quantum computing education platform consisting of: (1) C++ Quantum Simulator Backend - a lightweight, "
    "didactic quantum register simulator built from first principles; (2) Next.js Interactive Frontend - a modern "
    "web interface for visual circuit building and simulation; (3) Educational Documentation - comprehensive guides "
    "and learning materials.",
    body_style
))
story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Project Goals", heading2_style))
goals = [
    "✅ Demystify Quantum Computing through transparent, step-by-step implementation",
    "✅ Provide Visual Learning with interactive, drag-and-drop circuit building",
    "✅ Focus on Pedagogy over performance",
    "✅ Demonstrate Algorithmic Quantum Advantage with working examples",
    "✅ Eliminate External Dependencies (backend uses pure C++17, no quantum SDKs)"
]
for goal in goals:
    story.append(Paragraph(goal, body_style))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Key Statistics", heading2_style))
stats = [
    ["Qubits Supported", "1-5 (frontend), n-qubit (backend)"],
    ["Time Steps per Circuit", "8"],
    ["Quantum Gates", "15+ (H, X, Y, Z, S, T, CNOT, SWAP, M)"],
    ["Pre-built Templates", "10+ educational circuits"],
    ["Documentation", "5+ comprehensive guides"],
    ["Code Philosophy", "Pedagogical with extensive comments"],
]

stats_table = Table(stats, colWidths=[2.5*inch, 3.5*inch])
stats_table.setStyle(TableStyle([
    ('BACKGROUND', (0, 0), (-1, 0), colors.HexColor('#e8e8e8')),
    ('TEXTCOLOR', (0, 0), (-1, -1), colors.black),
    ('ALIGN', (0, 0), (-1, -1), 'LEFT'),
    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
    ('FONTSIZE', (0, 0), (-1, -1), 10),
    ('PADDING', (0, 0), (-1, -1), 6),
    ('GRID', (0, 0), (-1, -1), 1, colors.HexColor('#cccccc')),
]))

story.append(stats_table)

story.append(PageBreak())

# === ARCHITECTURE ===
story.append(Paragraph("2. Project Architecture", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("System Design Overview", heading2_style))
story.append(Paragraph(
    "Bhramanantdarshana consists of two main components that work together: "
    "a C++ backend providing the quantum simulation engine, and a Next.js frontend offering "
    "visual circuit building and real-time visualization. The architecture prioritizes clarity and "
    "educational value over raw performance.",
    body_style
))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Backend Architecture (C++)", heading2_style))
backend_components = [
    ("Quantum_registers_cl.cpp", "Core state vector management, normalization, measurement"),
    ("RegisterGates_cl.cpp", "Gate operations on quantum registers"),
    ("Gates_cl.cpp", "Single-qubit gate definitions and theory"),
    ("DeutscheAlgo_example.cpp", "Deutsch algorithm implementation with oracles"),
]

for component, description in backend_components:
    story.append(Paragraph(f"<b>{component}</b>: {description}", body_style))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Frontend Architecture (Next.js/React)", heading2_style))
frontend_components = [
    ("QuantumCircuitBuilderV2.tsx", "Main application logic and state management"),
    ("CircuitGridWithConnections.tsx", "Drop zones and visual connection lines"),
    ("GateLibrary.tsx", "Draggable gate palette with descriptions"),
    ("QubitVisualizer.tsx", "Bloch sphere and probability visualization"),
    ("TemplateGallery.tsx", "Pre-built educational circuits"),
    ("Toast.tsx", "User notifications"),
]

for component, description in frontend_components:
    story.append(Paragraph(f"<b>{component}</b>: {description}", body_style))

story.append(PageBreak())

# === CORE COMPONENTS ===
story.append(Paragraph("3. Core Components", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Backend Components: The Quantum Engine", heading2_style))

story.append(Paragraph("State Vector Representation", heading2_style))
story.append(Paragraph(
    "The quantum state is stored as a vector of 2^n complex amplitudes, where n is the number of qubits. "
    "Each amplitude corresponds to a basis state, with the amplitude's squared magnitude representing the "
    "measurement probability for that state.",
    body_style
))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Gate Operations", heading2_style))
story.append(Paragraph(
    "Single-qubit gates work by pairing amplitudes based on bit positions and applying the gate matrix. "
    "Multi-qubit gates like CNOT create entanglement by conditionally modifying state elements.",
    body_style
))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Measurement Mechanics", heading2_style))
story.append(Paragraph(
    "Measurement converts quantum probabilities into classical outcomes through probabilistic collapse. "
    "The simulator supports both destructive measurement (which collapses the state) and non-destructive "
    "sampling (for statistics gathering).",
    body_style
))

story.append(Spacer(1, 0.2*inch))

story.append(Paragraph("Frontend Components: The User Interface", heading2_style))

story.append(Paragraph("Circuit Builder", heading2_style))
story.append(Paragraph(
    "The main interface where users select qubits, drag gates from a library, and place them on a circuit. "
    "Supports 1-5 qubits with 8 time steps each. Real-time visualization of quantum states as gates are added.",
    body_style
))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Two-Qubit Connections", heading2_style))
story.append(Paragraph(
    "Interactive workflow for creating entangling gates. User drags a CNOT/SWAP, drops it on control qubit, "
    "then clicks target qubit to complete the connection. Visual lines show the relationship.",
    body_style
))

story.append(Spacer(1, 0.1*inch))

story.append(Paragraph("Real-Time Visualization", heading2_style))
story.append(Paragraph(
    "Multiple representation formats: Bloch sphere (geometric), probability bars (numerical), state vector "
    "(mathematical), and measurement results (classical outcomes). Helps learners understand from multiple perspectives.",
    body_style
))

story.append(PageBreak())

# === TECHNOLOGY STACK ===
story.append(Paragraph("4. Technology Stack", heading1_style))
story.append(Spacer(1, 0.15*inch))

tech_data = [
    ["<b>Component</b>", "<b>Technology</b>", "<b>Version</b>", "<b>Purpose</b>"],
    ["Backend", "C++", "C++17", "Core quantum simulation"],
    ["Backend", "STL", "Standard", "Containers & algorithms"],
    ["Backend", "Compiler", "MinGW/g++", "Compilation"],
    ["Frontend", "Next.js", "16.x", "React framework"],
    ["Frontend", "React", "19.2.x", "UI library"],
    ["Frontend", "TypeScript", "5.x", "Type safety"],
    ["Frontend", "Tailwind CSS", "4.x", "Styling"],
    ["Frontend", "react-dnd", "Latest", "Drag & drop"],
    ["Viz", "Python", "3.8+", "Data visualization"],
    ["Viz", "matplotlib", "Latest", "Plotting library"],
    ["VCS", "Git", "Latest", "Version control"],
]

tech_table = Table(tech_data, colWidths=[1.2*inch, 1.5*inch, 1*inch, 1.8*inch])
tech_table.setStyle(TableStyle([
    ('BACKGROUND', (0, 0), (-1, 0), colors.HexColor('#333333')),
    ('TEXTCOLOR', (0, 0), (-1, 0), colors.white),
    ('TEXTCOLOR', (0, 1), (-1, -1), colors.black),
    ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
    ('FONTSIZE', (0, 0), (-1, -1), 9),
    ('PADDING', (0, 0), (-1, -1), 6),
    ('GRID', (0, 0), (-1, -1), 1, colors.HexColor('#cccccc')),
    ('ROWBACKGROUNDS', (0, 1), (-1, -1), [colors.white, colors.HexColor('#f9f9f9')]),
]))

story.append(tech_table)

story.append(PageBreak())

# === DETAILED FEATURES ===
story.append(Paragraph("5. Detailed Features", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Feature 1: Interactive Drag-and-Drop Building", heading2_style))
story.append(Paragraph(
    "<b>User Journey:</b> Select qubit count → Drag gates from library → Drop on circuit grid → "
    "Build complete quantum circuit → Run simulation → View results.",
    body_style
))
story.append(Spacer(1, 0.08*inch))
story.append(Paragraph(
    "<b>Implementation:</b> Uses React DnD with HTML5 backend for smooth drag-and-drop. Drop zones "
    "update state immediately. Visual feedback shows gate placement in real-time.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Feature 2: Two-Qubit Gate Connections", heading2_style))
story.append(Paragraph(
    "<b>Workflow:</b> Drag CNOT/SWAP → Drop on control qubit → System prompts for target qubit → "
    "Click target qubit → Connection line drawn → Circuit ready.",
    body_style
))
story.append(Spacer(1, 0.08*inch))
story.append(Paragraph(
    "<b>Teaching Value:</b> Step-by-step interaction helps learners understand multi-qubit gate mechanics. "
    "Visual connection lines demonstrate entanglement relationships.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Feature 3: Pre-built Circuit Templates (10+)", heading2_style))

template_categories = [
    ("🔗 Entanglement", "Bell State, GHZ State, Entanglement + Measurement"),
    ("🎯 Algorithms", "Deutsch Algorithm (quantum advantage demonstration)"),
    ("⚛️ Gate Demos", "Quantum NOT, Superposition, Phase Flip, Pauli Gates, State Swap"),
    ("📏 Measurement", "Measurement Collapse, Entangled Measurement"),
]

for category, templates in template_categories:
    story.append(Paragraph(f"<b>{category}:</b> {templates}", body_style))

story.append(Spacer(1, 0.1*inch))
story.append(Paragraph(
    "Each template includes pre-configured gates, connections, and detailed explanation. Users can load "
    "instantly and modify to experiment.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Feature 4: Real-Time Visualization", heading2_style))
viz_formats = [
    ("Bloch Sphere", "Geometric representation of single-qubit states"),
    ("Probability Bars", "Visual |0⟩ and |1⟩ probability distribution"),
    ("State Vector", "Mathematical notation of quantum superposition"),
    ("Entanglement Badge", "Shows which qubits are entangled"),
    ("Measurement Results", "Classical outcomes from simulation"),
]

for fmt, desc in viz_formats:
    story.append(Paragraph(f"<b>{fmt}:</b> {desc}", body_style))

story.append(PageBreak())

# === USER GUIDE ===
story.append(Paragraph("6. User Guide", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Quick Start: 5 Minutes to First Circuit", heading2_style))
story.append(Spacer(1, 0.08*inch))

quick_start_steps = [
    "1. Navigate to http://localhost:3000",
    "2. Application loads with interactive circuit builder",
    "3. Keep default 1 qubit, or select 2-5 from dropdown",
    "4. Drag 'H' (Hadamard) gate to qubit 0, time step 0",
    "5. Click 'Run Simulation' button",
    "6. Watch Bloch sphere update showing superposition",
    "7. See 50% probability for |0⟩ and |1⟩",
]

for step in quick_start_steps:
    story.append(Paragraph(step, body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Common Tasks", heading2_style))

story.append(Paragraph("Task: Create Superposition", heading2_style))
story.append(Paragraph(
    "1. Keep 1 qubit. 2. Drag 'H' gate to position [0,0]. 3. Run simulation. "
    "4. Result: 50% chance of measuring 0 or 1.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph("Task: Create Bell State (Entanglement)", heading2_style))
story.append(Paragraph(
    "1. Select 2 qubits. 2. Drag 'H' to [0,0]. 3. Drag 'CNOT' to [0,1]. "
    "4. Click [1,1] to complete connection. 5. Run. Result: Qubits always measure same value.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph("Task: Load Pre-built Template", heading2_style))
story.append(Paragraph(
    "1. Click '⚡ Templates' button (bottom-right). 2. Browse available circuits. "
    "3. Click 'Load →' on desired template. 4. Circuit auto-loads with explanation.",
    body_style
))

story.append(PageBreak())

# === TECHNICAL DEEP DIVE ===
story.append(Paragraph("7. Technical Implementation", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Quantum State Representation", heading2_style))
story.append(Paragraph(
    "<b>State Vector Encoding:</b> For n qubits, we maintain 2^n complex amplitudes. Index i represents "
    "the binary state |b_n...b_2b_1b_0⟩. For example, in a 3-qubit system: index 0 = |000⟩, index 5 = |101⟩.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph(
    "<b>Normalization:</b> After each gate operation, the sum of squared magnitude of all amplitudes "
    "must equal exactly 1.0. This represents the conservation of probability.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Gate Implementation Pattern", heading2_style))
story.append(Paragraph(
    "Gates work by pairing amplitudes based on the target qubit's bit position. For each amplitude at index i "
    "with a 0 at the target position, we find the paired index j (with 1 at target position). We then apply the "
    "gate matrix to both amplitudes simultaneously.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph(
    "<b>Example (Hadamard):</b> For each pair of amplitudes, apply the matrix [[1, 1], [1, -1]]/√2. "
    "This creates superposition by mixing the two basis state amplitudes.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Measurement Mechanics", heading2_style))
story.append(Paragraph(
    "<b>Collapse Process:</b> 1) Calculate probability for each outcome: P(i) = |amplitude[i]|². "
    "2) Create cumulative distribution. 3) Sample using uniform random number. 4) Zero all other amplitudes. "
    "5) Renormalize to probability 1.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph(
    "<b>Non-Destructive Sampling:</b> Same as above but without modifying state—used for statistics gathering.",
    body_style
))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Two-Qubit Gate Mechanics", heading2_style))
story.append(Paragraph(
    "<b>CNOT Gate:</b> Controlled-NOT flips the target qubit if control is |1⟩. Implemented by iterating "
    "through state amplitudes and swapping conditionally based on control qubit value.",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph(
    "<b>SWAP Gate:</b> Exchanges states between two qubits. Implemented by permuting amplitude positions.",
    body_style
))

story.append(PageBreak())

# === INSTALLATION ===
story.append(Paragraph("8. Installation & Deployment", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Backend Setup (C++)", heading2_style))

story.append(Paragraph("Compilation (Windows with MinGW):", heading2_style))
story.append(Paragraph(
    "<font face=\"Courier\" size=\"9\">g++ -std=c++17 Quantum_registers_cl.cpp RegisterGates_cl.cpp "
    "DeutscheAlgo_example.cpp -o deutsch.exe</font>",
    body_style
))

story.append(Spacer(1, 0.08*inch))

story.append(Paragraph("Execution:", heading2_style))
story.append(Paragraph("<font face=\"Courier\" size=\"9\">./deutsch.exe</font>", body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Frontend Setup (Next.js)", heading2_style))

steps = [
    "1. <font face=\"Courier\">cd quantum-frontend</font>",
    "2. <font face=\"Courier\">npm install</font>",
    "3. <font face=\"Courier\">npm run dev</font>",
    "4. Visit http://localhost:3000",
]

for step in steps:
    story.append(Paragraph(step, body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Production Build", heading2_style))
story.append(Paragraph(
    "<font face=\"Courier\">npm run build</font> creates optimized production build. "
    "<font face=\"Courier\">npm run start</font> launches production server.",
    body_style
))

story.append(PageBreak())

# === ROADMAP ===
story.append(Paragraph("9. Future Roadmap", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Phase 1: Near Term (0-3 months)", heading2_style))
roadmap_near = [
    "Backend: Better random number generation (std::mt19937)",
    "Backend: Unit tests for all gate operations",
    "Frontend: Additional gate types (T, S, Rx, Ry, Rz)",
    "Frontend: 3D Bloch sphere visualization",
    "Frontend: Circuit save/load as JSON",
    "Documentation: Video tutorials",
]
for item in roadmap_near:
    story.append(Paragraph(f"• {item}", body_style))

story.append(Spacer(1, 0.12*inch))

story.append(Paragraph("Phase 2: Medium Term (3-6 months)", heading2_style))
roadmap_medium = [
    "Algorithm: Deutsch-Jozsa (full implementation)",
    "Algorithm: Grover's search algorithm",
    "Algorithm: Bernstein-Vazirani algorithm",
    "Feature: Quantum state tomography",
    "Feature: Entanglement entropy calculation",
]
for item in roadmap_medium:
    story.append(Paragraph(f"• {item}", body_style))

story.append(Spacer(1, 0.12*inch))

story.append(Paragraph("Phase 3: Long Term (6-12 months)", heading2_style))
roadmap_long = [
    "Platform: Cloud hosting (Vercel, AWS)",
    "Platform: User accounts and circuit sharing",
    "Physics: Quantum error correction basics",
    "Physics: Noise simulation models",
    "Gamification: Achievement badges and leaderboards",
]
for item in roadmap_long:
    story.append(Paragraph(f"• {item}", body_style))

story.append(PageBreak())

# === LEARNING OUTCOMES ===
story.append(Paragraph("10. Learning Outcomes", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("For Students Using This Platform:", heading2_style))

concepts = [
    "Quantum state vector representation and complex amplitudes",
    "Superposition principle and measurement collapse",
    "Single-qubit gates (H, X, Y, Z, S, T) and their effects",
    "Multi-qubit entanglement (CNOT, SWAP gates)",
    "Quantum algorithms demonstrating computational advantage",
    "Bell states and GHZ states for entanglement study",
    "Measurement correlation in entangled systems",
    "Linear algebra applications in quantum computing",
]

for concept in concepts:
    story.append(Paragraph(f"✓ {concept}", body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("For Educators Using This Platform:", heading2_style))

educator_benefits = [
    "Visual demonstration tool for abstract quantum concepts",
    "Interactive experimentation environment for students",
    "Ready-made circuit templates for structured lessons",
    "Immediate feedback and visual confirmation of concepts",
    "Customizable templates for problem-based learning",
]

for benefit in educator_benefits:
    story.append(Paragraph(f"◆ {benefit}", body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph("Key Competencies Developed:", heading2_style))

comp_data = [
    ["<b>Competency</b>", "<b>Level</b>", "<b>How Demonstrated</b>"],
    ["Quantum Mechanics", "Foundational", "Building and simulating circuits"],
    ["Algorithm Design", "Intermediate", "Deutsch algorithm implementation"],
    ["Linear Algebra", "Intermediate", "Gate matrices and operations"],
    ["Programming", "Intermediate", "Interactive circuit building"],
    ["Problem Solving", "Advanced", "Designing quantum circuits"],
]

comp_table = Table(comp_data, colWidths=[1.8*inch, 1.5*inch, 2.2*inch])
comp_table.setStyle(TableStyle([
    ('BACKGROUND', (0, 0), (-1, 0), colors.HexColor('#333333')),
    ('TEXTCOLOR', (0, 0), (-1, 0), colors.white),
    ('TEXTCOLOR', (0, 1), (-1, -1), colors.black),
    ('ALIGN', (0, 0), (-1, -1), 'LEFT'),
    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
    ('FONTSIZE', (0, 0), (-1, -1), 9),
    ('PADDING', (0, 0), (-1, -1), 6),
    ('GRID', (0, 0), (-1, -1), 1, colors.HexColor('#cccccc')),
    ('ROWBACKGROUNDS', (0, 1), (-1, -1), [colors.white, colors.HexColor('#f9f9f9')]),
]))

story.append(comp_table)

story.append(PageBreak())

# === CONCLUSION ===
story.append(Paragraph("Conclusion", heading1_style))
story.append(Spacer(1, 0.15*inch))

story.append(Paragraph(
    "<b>Bhramanantdarshana</b> represents a comprehensive effort to democratize quantum computing education. "
    "By combining a transparent C++ backend implementation with an intuitive interactive frontend, the platform "
    "enables learners to understand quantum mechanics from first principles.",
    body_style
))

story.append(Spacer(1, 0.12*inch))

story.append(Paragraph(
    "The project serves educators, students, and enthusiasts by providing:",
    body_style
))

story.append(Spacer(1, 0.08*inch))

benefits_final = [
    "🎯 Clear Implementation: Every quantum concept from first principles",
    "👁️ Visual Learning: Interactive UI supporting multiple learning styles",
    "📚 Documented Examples: 10+ pre-built circuits demonstrating concepts",
    "🔬 Real Algorithms: Deutsch algorithm showing quantum advantage",
    "🛠️ Extensible: Designed for addition of new gates and algorithms",
]

for benefit in benefits_final:
    story.append(Paragraph(benefit, body_style))

story.append(Spacer(1, 0.15*inch))

story.append(Paragraph(
    "This documentation provides everything needed for Bhuvnesh to create a compelling presentation of the "
    "Bhramanantdarshana quantum computing education platform.",
    body_style
))

story.append(Spacer(1, 0.3*inch))

story.append(Paragraph(
    f"<i>Generated: {datetime.now().strftime('%B %d, %Y at %H:%M')}</i>",
    body_style
))

story.append(Paragraph(
    "<i>Project Status: Active Development</i>",
    body_style
))

# Build PDF
doc.build(story)

print(f"✅ PDF generated successfully!")
print(f"📄 Output: {output_path}")
print(f"📊 Document contains comprehensive project documentation")
print(f"💡 Includes architecture, features, technical details, and learning outcomes")
print(f"🎯 Ready for presentation preparation by Bhuvnesh")
