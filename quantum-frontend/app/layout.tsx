import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import { DndProvider } from './DndProvider';

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "Quantum Circuit Simulator",
  description: "Interactive educational tool for learning quantum computing",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body className={inter.className}>
        <DndProvider>
          {children}
        </DndProvider>
      </body>
    </html>
  );
}
