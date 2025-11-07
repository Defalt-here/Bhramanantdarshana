'use client';

import { useEffect, useState } from 'react';

interface ToastProps {
  message: string;
  icon?: string;
  duration?: number;
  onClose: () => void;
}

export default function Toast({ message, icon = '✨', duration = 3000, onClose }: ToastProps) {
  const [isVisible, setIsVisible] = useState(true);

  useEffect(() => {
    const timer = setTimeout(() => {
      setIsVisible(false);
      setTimeout(onClose, 300); // Allow fade out animation
    }, duration);

    return () => clearTimeout(timer);
  }, [duration, onClose]);

  return (
    <div
      className={`fixed top-24 right-8 z-50 transition-all duration-300 ${
        isVisible ? 'opacity-100 translate-x-0' : 'opacity-0 translate-x-full'
      }`}
    >
      <div className="bg-white border-2 border-black text-black px-6 py-3 flex items-center gap-3 min-w-[280px]">
        <span className="text-xl font-extrabold">{icon}</span>
        <div className="flex-1">
          <p className="font-bold text-sm tracking-wide">{message}</p>
        </div>
        <button
          onClick={() => {
            setIsVisible(false);
            setTimeout(onClose, 300);
          }}
          className="text-black font-bold hover:bg-black hover:text-white px-2"
        >
          ✕
        </button>
      </div>
    </div>
  );
}
